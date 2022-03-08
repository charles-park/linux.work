Linux Server Image Screen saver off 방법(HDMI모니터 연결시 일정시간 후 Blank)

/dev/fb0의 screen saver 동작 off(BLANK시에는 잠시 살아나지만 일정시간 지난 후 다시 BLANK)
echo 0 > /sys/class/graphics/fb0/blank

fb에 커서 점멸 off
echo 0 > /sys/class/graphics/fbcon/cursor_blink

부팅시 아래의 명령어를 실행할 수 있도록 함. 
setterm -blank 0 -powersave off

예를들어 /etc/bash.bashrc에 넣는 경우 login할 때 마다 명령을 실행하게 됨. 

ssh로 접속시에는 아래의 에러가 발생됨
setterm: terminal xterm-256color does not support --blank
setterm: cannot (un)set powersave mode: Inappropriate ioctl for device

