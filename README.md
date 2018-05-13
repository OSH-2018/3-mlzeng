# 3-mlzeng
```
zml@zml-HP-Z420-Workstation:~/OSH3/mountpoint % ls -al
total 4
drwxr-xr-x 0 root root    0 Jan  1  1970 .
drwxrwxr-x 3 zml  zml  4096 May 13 20:20 ..
zml@zml-HP-Z420-Workstation:~/OSH3/mountpoint % echo helloworld > testfile
zml@zml-HP-Z420-Workstation:~/OSH3/mountpoint % ls -l
total 70136983718352
-rw-r--r-- 1 zml zml 11 May 23  4447078 testfile
zml@zml-HP-Z420-Workstation:~/OSH3/mountpoint % cat testfile
helloworld
zml@zml-HP-Z420-Workstation:~/OSH3/mountpoint % dd if=/dev/zero of=testfile bs=1M count=2000
2000+0 records in
2000+0 records out
2097152000 bytes (2.1 GB, 2.0 GiB) copied, 6.77809 s, 309 MB/s
zml@zml-HP-Z420-Workstation:~/OSH3/mountpoint % ls -l testfile
-rw-r--r-- 1 zml zml 2097152000 May 23  4447078 testfile
zml@zml-HP-Z420-Workstation:~/OSH3/mountpoint % dd if=/dev/urandom of=testfile bs=1M count=1 seek=10
1+0 records in
1+0 records out
1048576 bytes (1.0 MB, 1.0 MiB) copied, 0.00938299 s, 112 MB/s
zml@zml-HP-Z420-Workstation:~/OSH3/mountpoint % ls -l testfile
-rw-r--r-- 1 zml zml 11534336 May 23  4447078 testfile
zml@zml-HP-Z420-Workstation:~/OSH3/mountpoint % dd if=testfile of=/dev/null
22528+0 records in
22528+0 records out
11534336 bytes (12 MB, 11 MiB) copied, 0.0266505 s, 433 MB/s
zml@zml-HP-Z420-Workstation:~/OSH3/mountpoint % rm testfile
zml@zml-HP-Z420-Workstation:~/OSH3/mountpoint % ls -al
total 4
drwxr-xr-x 0 root root    0 Jan  1  1970 .
drwxrwxr-x 3 zml  zml  4096 May 13 20:20 ..
zml@zml-HP-Z420-Workstation:~/OSH3/mountpoint %

