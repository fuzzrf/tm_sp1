Trend Micro ServerProtect for Linux local overflow.



splx_kernel_module-3.0.1.0019-src.tar.gz, file module/bin/modops.c
```
long  ioctlMod(struct file *file , unsigned int cmd, unsigned long arg)
...
...
   switch (cmd)
    {
    case SIOCSETINIFIL:
		...
[1]	    o_max_exc_pid = kini.max_exc_pid;
        o_max_vsc_pid = kini.max_vsc_pid;

[2]     get_user(kini.max_exc_pid, (int *) & (init->max_exc_pid));
		...
[3]     if (o_max_exc_pid < kini.max_exc_pid)
        {
            newp = (pid_t *)kmalloc(sizeof(pid_t) *
                                    kini.max_exc_pid, GFP_ATOMIC);
            if (newp == NULL)
                WPRINTK("SPLXMOD: %d: %s: newp is NULL\n", cur_pid, cpsMethod);
[4]           memcpy(newp, exc_pid_ary, exc_pid_no * sizeof(pid_t));
            if (exc_pid_ary != NULL) kfree(exc_pid_ary);
            exc_pid_ary = newp;
            newp = NULL;
        }

```
Note that all max_exc_pid variables are signed ints, 'exc_pid_no' and 'kini' are globals.
To trigger the bug, we need to do the following:
1) call SIOCSETINIFIL ioctl with some positive value of max_exc_pid, lets say it's 1000
After that we need to call SIOCPUTEXCPID ioctl to actually populate 'exc_pid_ary' array,
as a result 'exc_pid_no' will be set to 1000.

2) call SIOCSETINIFIL ioctl with negative value of max_exc_pid arg, -1 for example
In this case, kini->max_exc_pid will be set to a negative value, but the 'exc_pid_no' and 'exc_pid_ary' remain unchanged, because check on line #3 fails

3) call SIOCSETINIFIL ioctl with some small value of max_exc_pid, let say it's 1
Now, when we hit line #3, o_max_exc_pid will be -1 and kini.max_exc_pid will be 1, so result will be true.
After that, 'newp' will be allocated (remember that kini.max_exc_pid = 1) and memcpy on line #4 will overflow it (exc_pid_no = 1000).




