# PIM Software Stack
This is about `PIM Software Stack`

Contact me on [iESLAB GyungMo Kim](mailto:7bvcxz@gmail.com) for more information.

``` bash
git clone https://github.com/7bvcxz/PIM_SoftwareStack
cd PIM_SoftwareStack
```

## 간단 사용 설명서
``` bash
./build_so.sh   # (make clean && make so)
cp libpimss.so tutorial
cd tutorial
./build.sh      # (make 
```

## Installing PIM Software Stack
* The implementation was compiled and tested with Ubuntu 18.04, gcc, g++ #TODO#

``` bash
./test.sh    # (make clean && make fpga && make)
./test gemv  # Test GEMV
./test add   # Test ADD
./test mul   # Test MUL
```

### PIM Software Stack Install 시에 주의사항
PIM Software Stack에서는 다양한 모듈과 연동하여 사용가능하도록 여러가지 모드로 Install할 수 있습니다.
1. PIM Software Stack + PIM Memory
2. PIM Software Stack + PIMsim (PIM Memory에 대한 Simulator)
3. PIM Software Stack + FPGA

어떤 모드로 Install할지 설정한 이후에 Install을 수행해야 합니다.

모드는 pim_config.h의 45~49번째 줄을 통해서 설정할 수 있습니다.  
1번 모드는 실제 PIM Memory가 필요하기 때문에, 보통은 2번, FPGA가 존재할 시에는 3번 모드를 사용합니다.  
PIMsim은 PIM Software Stack이 생성한 메모리 커맨드에 따라 PIM Memory의 동작을 흉내내어 연산을 수행하고 연산 결과를 반환합니다.  
이를 통해 연산을 PIM Memory의 구조에 맞게 PIM Software Stack을 정확하게 설계했는지 확인할 수 있습니다.  
현재는 2번 모드를 사용하다가, 추후 FPGA 사용시에 3번 모드를 사용하여 실험해볼 수 있습니다.  
아래는 2번 모드로 Install하는 설정법입니다. (Default 설정되어 있음)

``` cpp
// #define fpga_mode
// #define debug_mode
#define compute_mode
```


## PyTorch + PIM Software Stack
### Installing PyTorch + PIM Software Stack
* The implementation was compiled and tested with #TODO#
``` bash
python3 run.py
or
sudo python3 setup.py install
cp build/lib.linux-x86_64-3.6/* .")   # Different in other python versions (Need to check file name)
python3 app.py  # Run Test Code
```

### How To Use
Call pim library and use pim.to_pim(model)
``` python
import pim_library as pim

model = MyModel()
model = pim.to_pim(model)

output = model(input)
# It's over
```

## How PIM Software Stack Works
#TODO#



