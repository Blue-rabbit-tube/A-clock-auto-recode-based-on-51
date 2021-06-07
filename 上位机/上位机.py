import serial
import time
import csv

#打开串口
serialPort="COM5" #串口
baudRate=9600 #波特率
ser=serial.Serial(serialPort,baudRate,timeout=0.5) 
print("参数设置：串口={{}} ，波特率={{}}".format(serialPort,baudRate))
f = open('试验记录2.csv','a+',encoding='utf-8',newline="")
record = csv.writer(f)
record.writerow(['number','time/s'])
i_ok=1

#收发数据
while(1):
    p_aa=[]
    p=str(ser.readline())
    if p[2:-5]!="":
        s_back=list(p[2:-5])
        # print(s_back)
        for i in s_back:                            #开始出入栈问题
            p_aa.insert(0,i)
            p_ok=float(''.join(p_aa))/1000
        print(str(p_ok)+"秒")
        record.writerow([i_ok,p_ok])
        i_ok=i_ok+1
        if i_ok > 21:
            input()
            f.close()
            ser.close()
            exit()