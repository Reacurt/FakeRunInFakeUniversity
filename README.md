# FakeRunInFakeUniversity

###### 某大专智障体育自动化打卡思路分享

    本分享包含的内容有：网络抓包、小程序逆向、微信逆向。

### 一、抓包

#### 1、使用fiddler+移动端微信抓包

fiddler的基本使用请参考 [fiddler入门](https://zhuanlan.zhihu.com/p/410150022)。
没有经验请自行搜索 [fiddler抓包手机](https://www.bing.com/search?q=fiddler%E6%8A%93%E5%8C%85%E6%89%8B%E6%9C%BA&qs=n&form=QBRE&sp=-1&lq=0&pq=fiddler%E6%8A%93%E5%8C%85%E6%89%8B%E6%9C%BA&sc=10-11&sk=&cvid=5960BB3CE07349CEA21F08A998BFF7B3&ghsh=0&ghacc=0&ghpl=).

安卓7以后想要使用fiddler抓到https的包需要将fiddler的证书添加到系统证书。这里可以使用雷电模拟器省去root的麻烦。
导出fiddler的证书文件,cmd中运行命令将证书转换为pem文件：

```shell
openssl x509 -in FiddlerRoot.cer -inform DER -out certificate.pem -outform PEM
```
计算pem证书哈希：
```shell
openssl x509 -inform PEM -subject_hash_old -in certificate.pem
```
将 **pem证书** 重命名为 **{哈希值}.0** :

![图片](pic/抓包/1.png "将pem文件重命名为.0文件")


然后将该.0文件移动到安卓机 **/system/etc/security/cacerts/** 目录下，之后设置好安卓机的代理，就可以愉快的抓包了。


#### 2、小程序打卡过程分析

##### (1)检查绑定
```http
GET https://sport.cqupt.edu.cn/wxapp/wxUnifyId/checkBinding?wxCode=031xAh0w3VZJj03kGj2w3QDsZX2xAh0j HTTP/1.1
Host: sport.cqupt.edu.cn
Connection: keep-alive
charset: utf-8
User-Agent: Mozilla/5.0 (Linux; Android 10; MI 8 Lite Build/QKQ1.190910.002; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/107.0.5304.141 Mobile Safari/537.36 XWEB/5017 MMWEBSDK/20230202 MMWEBID/9800 MicroMessenger/8.0.33.2320(0x28002151) WeChat/arm64 Weixin NetType/WIFI Language/zh_CN ABI/arm64 MiniProgramEnv/android
content-type: application/json
Accept-Encoding: gzip,compress,br,deflate
Referer: https://servicewechat.com/wx1680cca9a19ee3c8/16/page-frame.html
```

GET请求传参wxcode，这个wxcode是小程序调用微信提供的接口wx.login()获取，是作为微信用户登录的一个临时凭证，五分钟的有效期，只能使用一次，并且wx.login()调用时使用的是微信自己的通信协议，所以正常抓包抓不到。返回的结果也只有绑定成功和绑定失败，用于确定当前账号是否已经绑定过学生。

##### （2）绑定学生
```http
GET https://sport.cqupt.edu.cn/wxapp/wxUnifyId/wecatBindUnifyId?unifyId=167xxxx&studentNo=2029xxxxxx&wxCode=021tQn1w3G8Qk03T031w3UJtpi3tQn12 HTTP/1.1
Host: sport.cqupt.edu.cn
Connection: keep-alive
charset: utf-8
User-Agent: Mozilla/5.0 (Linux; Android 10; MI 8 Lite Build/QKQ1.190910.002; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/107.0.5304.141 Mobile Safari/537.36 XWEB/5017 MMWEBSDK/20230202 MMWEBID/9800 MicroMessenger/8.0.33.2320(0x28002151) WeChat/arm64 Weixin NetType/WIFI Language/zh_CN ABI/arm64 MiniProgramEnv/android
content-type: application/json
Accept-Encoding: gzip,compress,br,deflate
token: [object Undefined]
Referer: https://servicewechat.com/wx1680cca9a19ee3c8/16/page-frame.html

HTTP/1.1 200 OK
Server: rums/b
Date: Mon, 20 Mar 2023 11:28:03 GMT
Content-Type: application/json
Transfer-Encoding: chunked
Connection: keep-alive
Vary: Origin
Vary: Access-Control-Request-Method
Vary: Access-Control-Request-Headers
79
{"msg":"请求成功","code":"10200","data":{"msg":"绑定成功","token":"yrngxCDuNMxBejc2qqLHYTCqk7d7NgmXWNivEanO5vM"}}
0
```

GET请求传参统一认证吗、学号和wxcode，学号和认证号标识学生，wxcode标识微信用户，通过后会返回一个token作为小程序使用的一个临时凭证。有效期比wxcode长得多，换绑或者长时间没有和学校服务器通信都会失效。

##### （3）开始打卡

```http
POST https://sport.cqupt.edu.cn/wxapp/sportRecord/sport/start HTTP/1.1
Host: sport.cqupt.edu.cn
Connection: keep-alive
Content-Length: 161
charset: utf-8
User-Agent: Mozilla/5.0 (Linux; Android 10; MI 8 Lite Build/QKQ1.190910.002; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/107.0.5304.141 Mobile Safari/537.36 XWEB/5017 MMWEBSDK/20230202 MMWEBID/9800 MicroMessenger/8.0.33.2320(0x28002151) WeChat/arm64 Weixin NetType/WIFI Language/zh_CN ABI/arm64 MiniProgramEnv/android
content-type: application/json
Accept-Encoding: gzip,compress,br,deflate
token: 1_sr3fCDy8qL31dOG9AkOnDL6rP_jl5DaWuOkvc3SV4
Referer: https://servicewechat.com/wx1680cca9a19ee3c8/16/page-frame.html

{"longitude":106.61017768012152,"latitude":29.532591417100694,"placeName":"太极运动场","placeCode":"T1005","collectTime":"2023-3-20 10:58:58","isValid":"1"}

HTTP/1.1 200 OK
Server: rums/b
Date: Mon, 20 Mar 2023 02:59:03 GMT
Content-Type: application/json
Transfer-Encoding: chunked
Connection: keep-alive
Vary: Origin
Vary: Access-Control-Request-Method
Vary: Access-Control-Request-Headers
2c7
{"msg":"请求成功","code":"10200","data":{"id":21312,"sportRecordNo":"RBL054sdfsdfOD8AMO4J94YSdfsdfKQJZ","unifyId":"167xxxx","studentName":"李二狗","studentNo":"2029xxxxxx","sex":"6","deptName":"意大利面与42号混凝土学院","deptCode":"30800","openid":"sfdfsdfsdfsdfdsfasdask","placeName":"太极运动场","placeCode":"T1005","mileage":0,"timeConsuming":0,"startTime":"2023-03-20 10:58:44","endTime":null,"isValid":null,"reason":null,"createBy":"167xxxx","createName":"李二狗","createTime":"2023-03-20T10:58:44.248+08:00","updateBy":"167xxxx","updateName":"李二狗","updateTime":"2023-03-20T10:58:44.248+08:00","remark":null,"sportDateStr":null,"sportHourStr":null,"reasonLabel":null}}
0
```
POST请求，传参如上，header里需要带有token做为认证信息。返回内容中：**sportRecordNo** 将会作为后台数据记录和计算的一个标识id。
