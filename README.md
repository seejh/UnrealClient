## 게임 클라이언트
게임서버 포트폴리오 클라이언트 코드입니다.<br/>

**기술 및 환경** : Windows, Unreal4, C++<br/>

**프로그램 설명**<br/>
![image](https://github.com/seejh/UnrealClient/assets/152791315/f9a1a6d0-9956-4936-9e7e-e2a4fbb2d36c)
<br/>

**기본 동작**<br/>
네트워크 스레드에서 패킷을 받고 패킷 큐에 적재합니다. 게임 스레드에서 주기적으로 패킷큐에 적재된 패킷들과 플레이어 입력을 업데이트 후 전송합니다.


