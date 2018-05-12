gcc -Ofast -D_FILE_OFFSET_BITS=64 -o oshfs oshfs.c -lfuse
scp oshfs station:~/OSH3/
ssh -t station "~/OSH3/oshfs ~/OSH3/mountpoint"
ssh -t station "cd ~/OSH3/mountpoint; zsh"
ssh -t station "sudo umount ~/OSH3/mountpoint"
