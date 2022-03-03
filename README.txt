Linux Base image로 ODROID Boards에 사용하는 테스트 소스들 모음

git commit에 관한 내용:
  source를 수정하고 commit하기 위하여서는 access tocken을 password 대신 입력하도록 변경되었다.
  access token 얻는 방법 : github/charles-park/ 접속 및 login
  User설정의 developer setting에 보면 personal access tokens가 있고 유효시간 및 option을 설정할 수 있다.

설치 package : build-essential, i2c-tools, netutils, gpiod, git, usbutils, vim...
설치 라이브러리 : git clone https://github.com/hardkernel/wiringPi
