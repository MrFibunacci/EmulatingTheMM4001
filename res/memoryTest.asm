; store Hello World in Memory (starting address 0x21) and copy it to somewere else (starting adress 0xE0)

; initiallize Pointer Adress:
INA 2
MOV PH,A
INA 1
MOV PL,A

; H
INA 0x4
STO A AutoInc
INA 0x8
STO A AutoInc

; e
INA 0x6
STO A AutoInc
INA 0x5
STO A AutoInc

; l
INA 0x6
STO A AutoInc
INA 0xc
STO A AutoInc

; l
INA 0x6
STO A AutoInc
INA 0xc
STO A AutoInc

; o
INA 0x6
STO A AutoInc
INA 0xf
STO A AutoInc

; SPACE
INA 0x2
STO A AutoInc
INA 0x0
STO A AutoInc

; W
INA 0x5
STO A AutoInc
INA 0x7
STO A AutoInc

;o
INA 0x6
STO A AutoInc
INA 0xf
STO A AutoInc

; r
INA 0x7
STO A AutoInc
INA 0x2
STO A AutoInc

; l
INA 0x6
STO A AutoInc
INA 0xc
STO A AutoInc

; d
INA 0x6
STO A AutoInc
INA 0x4
STO A AutoInc