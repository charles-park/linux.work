/dev/fb0의 screen saver 동작 off
echo 0 > /sys/class/graphics/fb0/blank

fb에 커서 점멸 off
echo 0 > /sys/class/graphics/fbcon/cursor_blink
