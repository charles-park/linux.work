ODROID 16x2 LCD Shield를 사용하여 Network IP 상태를 표시함.

Button1 = 현재 시스템 시간표시, Button2 = Network 상태표시(IP/LinkSpeed)

필요한 Package = build-essential, wiringPi, gpiod

service 파일의 설치(파일 내용안에 실행파일 폴더 및 파일이름을 확인, permission도 확인하여야 함)
    sudo cp NetInfoDisplay.service /etc/systemd/system/
service enable
    sudo systemctl enable NetInfoDisplay
service disable
    sudo systemctl disable NetInfoDisplay
