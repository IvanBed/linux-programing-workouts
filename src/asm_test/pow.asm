
;x86-64 Assembly Power Function
;Описание
;Рекурсивная реализация возведения в степень на ассемблере x86-64.

pow:
movq %RSI, %RCX
cmpq $0 ,%RSI
je null_power
movq %RDI, %RAX

do:
decq %RCX
cmpq $0, %RCX
je end
mulq %RDI
jmp do

end:
retq

null_power:
movq $1, %RAX
retq
