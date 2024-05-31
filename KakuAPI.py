from sys import argv
import os
import time
from typing import TextIO


class KakuAPI():
    __file = None
    IN_FILE = "C:\\Users\\27746\\AppData\\Local\\Temp\\KakuBuffer\\in.buf"
    OUT_FILE = "C:\\Users\\27746\\AppData\\Local\\Temp\\KakuBuffer\\out.buf"

    def OpenFileW(self) -> TextIO:
        while (True):
            try:
                self.__file = open(self.IN_FILE, "w", encoding="gbk")
                pass
            except PermissionError as err:
                print(err.__str__())
                pass
            else:
                break
        return self.__file

    def OpenFileR(self) -> TextIO:
        while (True):
            try:
                self.__file = open(self.OUT_FILE, "r", encoding="gbk")
                pass
            except PermissionError as err:
                print(err.__str__())
                pass
            else:
                break
        return self.__file

    def ReadData(self) -> str:
        self.__file = self.OpenFileR()
        data = self.__file.read()
        self.__file.close()
        return data

    def SendData(self, cmd: str):
        self.__file = self.OpenFileW()
        self.__file.write(str(int(time.time())) + " " + cmd)
        print(str(int(time.time())) + " " + cmd)
        self.__file.close()
        return

    def RefrushW(self):
        _ = open(self.IN_FILE, "w")
        _.close()
        return

    def RefrushR(self):
        _ = open(self.OUT_FILE, "w")
        _.close()
        return

    pass


if __name__ == "__main__":
    cmd = ""
    kk = KakuAPI()
    #os.system("start Reader.exe")
    while True:
        cmd = input(">>")
        kk.SendData(cmd)
        time.sleep(0.5)
        #os.system("pause")
        print(kk.ReadData())
        kk.RefrushW()
        kk.RefrushR()
        if cmd == "exit":
            break
        pass
    pass
