import requests
import aiohttp
import datetime
import json
import random
import base64
from Crypto.Cipher import PKCS1_v1_5 as Cipher_pksc1_v1_5
from Crypto.PublicKey import RSA
from Crypto.Cipher import AES


async def get_token(unifyID,stu_num,wx_code):
    bind_data={
            "unifyId":unifyID,
            "studentNo":stu_num,
            "wxCode":wx_code
        }
    header={
        "Host": "sport.cqupt.edu.cn"
        ,"Connection": "keep-alive"
        ,"charset": "utf-8"
        ,"User-Agent": "Mozilla/5.0 (Linux; Android 10; MI 8 Lite Build/QKQ1.190910.002; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/107.0.5304.141 Mobile Safari/537.36 XWEB/5017 MMWEBSDK/20230202 MMWEBID/9800 MicroMessenger/8.0.33.2320(0x28002151) WeChat/arm64 Weixin NetType/WIFI Language/zh_CN ABI/arm64 MiniProgramEnv/android"
        ,"content-type": "application/json"
        ,"Accept-Encoding": "gzip,compress,br,deflate"
        ,"Referer": "https://servicewechat.com/wx1680cca9a19ee3c8/16/page-frame.html"

    }
    res=requests.get(url="https://sport.cqupt.edu.cn/wxapp/wxUnifyId/wecatBindUnifyId",params=bind_data,headers=header)
    res=res.json()
    if res["msg"]=="请求成功" and res["data"]["msg"]=="绑定成功":
            return res["data"]["token"]
    else:
        print("绑定失败")
        return "绑定失败"

class FakeRun(object):
    def __init__(self,token) -> None:
        self.start_url="https://sport.cqupt.edu.cn/wxapp/sportRecord/sport/start"
        self.savelist_url="https://sport.cqupt.edu.cn/wxapp/sportRecord/point/saveList"
        self.endlist_url="https://sport.cqupt.edu.cn/wxapp/sportRecord/sport/endList"
        self.getuser_url="https://sport.cqupt.edu.cn/wxapp/wxUnifyId/getUser"
        self.unlock_url="https://sport.cqupt.edu.cn/wxapp/wxUnifyId/binding/unlock"
        self.header={
        "Host": "sport.cqupt.edu.cn"
        ,"Connection": "keep-alive"
        ,"charset": "utf-8"
        ,"User-Agent": "Mozilla/5.0 (Linux; Android 10; MI 8 Lite Build/QKQ1.190910.002; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/107.0.5304.141 Mobile Safari/537.36 XWEB/5017 MMWEBSDK/20230202 MMWEBID/9800 MicroMessenger/8.0.33.2320(0x28002151) WeChat/arm64 Weixin NetType/WIFI Language/zh_CN ABI/arm64 MiniProgramEnv/android"
        ,"content-type": "application/json"
        ,"Accept-Encoding": "gzip,compress,br,deflate"
        ,"token": token
        ,"Referer": "https://servicewechat.com/wx1680cca9a19ee3c8/16/page-frame.html"
        ,}
        self.old_start_time_tdaiji="2023-3-24 11:52:2"
        self.old_start_time_fenghua="2023-3-24 10:1:45"
        self.new_start_time=self.get_now_time()
        self.session=aiohttp.ClientSession(headers=self.header)
        self.get_public_key()
        self.aeskey=''.join(random.choice("QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjkzxcvbnm1234567890") for i in range(16))
    def __del__(self):
        self.session.close()
    def get_now_time(self):
        now=datetime.datetime.now()
        formatted_time=now.strftime("%Y-%m-%d %H:%M:%S")
        return formatted_time
    
    def get_new_time(self,time1,time2,time3):
        dt1 = datetime.datetime.strptime(time1, "%Y-%m-%d %H:%M:%S")
        dt2 = datetime.datetime.strptime(time2, "%Y-%m-%d %H:%M:%S")
        dt3 = datetime.datetime.strptime(time3, "%Y-%m-%d %H:%M:%S")
        delta = dt2 - dt1
        new_time = dt3 + delta
        return new_time.strftime("%Y-%m-%d %H:%M:%S")
    
    def RSAencrpt(self):
        public_key = '-----BEGIN PUBLIC KEY-----\n' + self.publickey + '\n-----END PUBLIC KEY-----'
        rsakey = RSA.importKey(public_key)
        cipher = Cipher_pksc1_v1_5.new(rsakey)
        cipher_text = base64.b64encode(cipher.encrypt(self.aeskey.encode()))
        return cipher_text.decode()

    def AesEncryptECB(self,n):
        r = self.aeskey.encode()
        t = n.encode()
        cipher = AES.new(r, AES.MODE_ECB)
        # 对明文进行填充
        length = 16
        count = len(t)
        padding = length - (count % length)
        t += bytes([padding]) * padding
        # 加密
        ciphertext = cipher.encrypt(t)
        return base64.b64encode(ciphertext).decode('utf-8')

    
    async def get_record_code(self,playground):
        start_data_taiji={
            "longitude":106.60953206380208,
            "latitude":29.53216579861111,
            "placeName":"太极运动场",
            "placeCode":"T1005",
            "collectTime":self.new_start_time,
            "isValid":"1"}
        staet_data_fenghua={
            "longitude":106.60798258463542,
            "latitude":29.532691514756944,
            "placeName":"风华运动场",
            "placeCode":"T1001",
            "collectTime":self.new_start_time,
            "isValid":"1"}
        if playground=="太极":
            start_data=start_data_taiji
        else:
            start_data=staet_data_fenghua
        # res=requests.post(url=self.start_url,headers=self.header,data=json.dumps(start_data),verify=False)
        # res=res.json()
        async with self.session.post(url=self.start_url,data=json.dumps(start_data))as res:
            res=await res.text()
        try:
            res=json.loads(res)
        except json.JSONDecodeError:
            print("开始请求解码失败")
            res = {"msg": "失败"}
        if res["msg"]=="请求成功":
            return res["data"]["sportRecordNo"]
        else:
            print("开始失败")
            return "开始失败"
    
    def get_public_key(self):
        res=requests.get(url=self.getuser_url,headers=self.header)
        res=res.json()
        if res["msg"]=="请求成功":
            self.publickey=res["data"]["publicKey"]
            return res["data"]["publicKey"]
        else:
            print("公钥失败")
            return "公钥失败"
    # def upload_data(record_code):
    #     with open (r"C:\Users\Reacurt\Desktop\mcl\saya\modules\dailyRunning\locate.txt","r",encoding="utf-8")as f:
    #         locate_data=json.load(f)
    #     for i in locate_data:
    #         temp_data=i
    #         temp_data=json.loads(temp_data)
    #         for j in temp_data:
    #             j["sportRecordNo"]=record_code
    #             j["collectTime"]=get_new_time(old_start_time,j["collectTime"],new_start_time)
    #         res=requests.post(url=savelist_url,data=json.dumps(temp_data),headers=header)
    #         res=res.json()
    #         if res["msg"]=="请求成功":
    #             print("耗时：",str(res["data"]["timeConsuming"])+" 里程："+str(res["data"]["mileage"]))
    #             time.sleep(0.05)
    #             continue
    #         else:
    #             return False
    #     return True
    
    async def upload_data(self,playground,record_code):
        if  playground=="太极":
            path=r"modules/dailyRunning/taiji.txt"
            rand_end=random.randint(362,459)
            old_start_time=self.old_start_time_tdaiji
        else:
            path=r"modules/dailyRunning/fenghua.txt"
            rand_end=random.randint(364,461)
            old_start_time=self.old_start_time_fenghua
        with open (path,"r",encoding="utf-8")as f:
            locate_data=json.load(f)
        total_data=[]
        # for i in locate_data:
        for i in range(rand_end):
            temp_data=locate_data[i]
            temp_data=json.loads(temp_data)
            for j in temp_data:
                j["sportRecordNo"]=record_code
                j["collectTime"]=self.get_new_time(old_start_time,j["collectTime"],self.new_start_time)
            # total_data=total_data+temp_data
            up_data={
                "data":"dsdsd",
                "aesKey":self.RSAencrpt()
            }
            C=self.AesEncryptECB(json.dumps({ "sportPointList":  temp_data}))
            y=self.RSAencrpt()
            # res=requests.post(url=self.savelist_url+"?data="+C+"&aesKey="+y,data=json.dumps(up_data),headers=self.header)
            # res=res.json()
            surl = self.savelist_url+"?data="+C+"&aesKey="+y
            async with self.session.post(url=surl, data=json.dumps(up_data), headers=self.header) as res:
                res = await res.text()
            try:
                res = json.loads(res)
            except json.JSONDecodeError:
                print("上传解码失败")
            
        
        temp_end_data=locate_data[rand_end+1]
        temp_end_data=json.loads(temp_end_data)
        for k in temp_end_data:
            k["sportRecordNo"]=record_code
            k["collectTime"]=self.get_new_time(old_start_time,k["collectTime"],self.new_start_time)
        temp_end_data=[temp_end_data,]
        self.end_data=temp_end_data
        if res["msg"] == "请求成功":
            print(res["data"]["studentName"] + " 耗时：" +str(res["data"]["timeConsuming"]) + " 里程：" + str(res["data"]["mileage"]))
            return res["data"]["studentName"] + " 耗时：" +str(res["data"]["timeConsuming"]) + " 里程：" + str(res["data"]["mileage"])
        else:
            print("上传失败")
            return('上传失败')
                
    
    async def get_end(self):
        end_data={
            "sportPointRequests":self.end_data
        }
        # res=requests.post(url=self.endlist_url,headers=self.header,data=json.dumps(end_data))
        # res=res.json()
        async with self.session.post(url=self.endlist_url, data=json.dumps(end_data), headers=self.header) as res:
            res = await res.text()
        try:
            res = json.loads(res)
        except json.JSONDecodeError:
            print("结束解码失败")
            res = {"msg": "失败"}
        if res["msg"]=="请求成功":
            print("成功")
            return "成功：请在"+self.end_data[0][-1]["collectTime"]+" 前完成四次跑步打卡"
        else:
            print("结束失败")
            return "结束失败"
    
    async def unlock(self,unifyID,stu_num,wx_code):
        unlock_data={
            "wxCode":wx_code,
            "studentNo":stu_num,
            "unifyId":unifyID}
        res=requests.put(url=self.unlock_url,data=json.dumps(unlock_data),headers=self.header)
        res=res.json()
        if res["msg"]=="请求成功":
                return "完成"
        else:
            print("解绑失败")
            return "解绑失败"
