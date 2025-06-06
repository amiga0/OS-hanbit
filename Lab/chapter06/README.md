# 실습과제 6장 \) 파일 시스템 설계 및 구현

간단한 파일 시스템(Simple File system)을 설계하고 구현해 보자. 파일 시스템이 탑재된 저장 매체는 메타 데이터와 데이터를 저장하는 영역을 구분하며, 이를 정의하는 자료구조는 fs.h 헤더 파일에 포함되어 있다.

구현할 파일 시스템의 구조는 다음과 같다.

1. 메타 데이터 영역
    - 슈퍼 블록(super\_block): 파일 시스템 자체 정보를 저장한다.
    - inode 배열(inode\_table): 파일의 인덱스 정보를 저장한다.

2. 파일 관련 구조체  
    - inode 구조체: 각 파일의 메타 데이터를 포함한다. 파일 크기, 종류, 데이터 블록 정보 등을 저장한다.
    - 디렉터리 파일(dentry 구조체): 파일 목록을 관리하며, 파일명과 해당 파일의 inode 번호를 저장한다. inode\_table 배열의 인덱스가 해당 파일의 inode 번호를 의미한다.

3. 루트 디렉터리(/): 최상위 경로이며, inode 번호는 2번이다. 해당 정보는 슈퍼 블록에 저장한다.

## 응용과제 6-1) 디렉토리 파일 읽기

fs.h 헤더 파일 구조에 따라 저장된 파티션 이미지 파일(disk.img)에 저장된 내용을 읽어와 루트 디렉터리의 파일들을 출력한다. 이를 위해 다음 단계를 수행한다.

1. 루트 디렉터리 파일(inode 2번)을 연다.
2. 디렉터리 파일에서 각 파일의 inode 정보를 찾는다.
3. 각 inode 정보를 읽고 파일 크기를 확인한다.
4. inode에 저장된 데이터 블록을 순차적으로 읽는다.
5. 읽어들인 데이터를 dentry 형식에 맞게 분할하여 파일 목록을 출력한다.

- 참고로 disk.img.hex는 disk.img 파일을 16진수로 변환한 텍스트 파일이다.

## 응용과제 6-2) 파일 내용 읽기

루트 디렉터리에 저장된 file\_3 파일의 내용을 읽어 출력해보자. 이를 위해 다음 단계를 구현한다.

1. 루트 디렉터리 파일을 읽는다.
2. 디렉터리 파일에서 file\_3 파일의 inode 번호를 찾는다.
3. inode\_table에서 해당 inode 정보를 읽는다.
4. 파일 크기를 확인한 후 데이터 블록을 순차적으로 읽는다.
5. 읽어들인 데이터를 하나의 버퍼에 복사한 후 화면에 출력한다.

## 응용과제 6-3) 파일에 데이터 저장

파티션 이미지 파일을 열어, 파일을 읽고 데이터를 추가로 작성한 후, 저장 여부를 확인하는 기능을 구현해 보자.

현재 파일 시스템 이미지에는 데이터 블록의 사용 현황을 관리하는 자료구조가 포함되어 있지 않다. 이를 해결하기 위해 슈퍼 블록의 padding 영역을 활용하여 다음 단계로 프리 블록 맵을 구성한다.

1. 파일을 열고 기존 데이터를 읽는다.  
2. 새로운 데이터를 추가로 작성한다.  
3. 쓰기 작업이 정상적으로 수행되었는지 확인한다.  
4. 프리 블록 맵 활용한다.  
    - 사용 중인 데이터 블록은 1, 사용하지 않은 데이터 블록은 0으로 설정한다.  
    - 쓰기 과정에서 비어 있는 블록을 찾아 동적으로 할당 가능하도록 구현한다.

## 응용과제 6-4) 여러 사용자의 파일 접근 제어

여러 명의 사용자가 파일을 읽거나 쓸 수 있도록 파일 오픈 인터페이스를 정의하고, 읽기/ 쓰기 접근을 제어 기법을 다음과 같이 구현해 보자.

1. 파일 오픈 시 사용자가 읽기 또는 쓰기 플래그를 지정 가능하도록 인터페이스 제공  
    - O\_READ: 여러 사용자가 동시에 파일을 읽을 수 있다.  
    - O\_WRITE: 한 번에 한 사용자만 파일을 쓸 수 있도록 제어한다.  
    - O\_RDWR: 동시에 읽고 쓸 수 있으나 동기화가 필요하다.  

2. 파일 접근 동기화 구현  
    - 쓰기 중 다른 사용자가 접근하지 못하도록 락(Lock)을 적용한다.  
    - 뮤텍스(Mutex) 또는 세마포어(Semaphore) 활용해 동기화를 처리한다.  
    - 읽기-쓰기 락 (RW Lock) 적용하여 동시 읽기 허용, 쓰기 충돌 방지를 적용한다.  
    - 파일 닫기 (close()) 시 잠금 해제 및 접근 가능 상태로 변경한다.
