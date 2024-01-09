import frida
import time
import threading
def on_message(message, data):
    if message['type'] == 'send':
        print(message['payload'])

target_process_name="WeChat.exe"
process=frida.attach(target_process_name)
source="""
var weChatAppExModule = Process.findModuleByName("WeChatWin.dll");
var base = weChatAppExModule.base;
var codeAddr = base.add(0xFC7A30) 
var codeAddr_ = base.add(0xFD8071) 
var codeCall = new NativeFunction(codeAddr, 'char', ['pointer', 'pointer', 'pointer', 'pointer', 'pointer']);

var ind = ptr(0x69);
var id = ptr(0x21);
var loginTypeString = Memory.allocUtf8String(JSON.stringify({"login_type": 1, "version_type": 1}));
var loginString = Memory.allocUtf8String('login');
var appidString = Memory.allocUtf8String('wx1680cca9a19ee3c8');

var code = codeCall(appidString, loginString, loginTypeString, id, ind);
Interceptor.attach(codeAddr_, {
    onEnter: function (args) {
        //console.log(this.context.eax.readUtf8String())
        var output = this.context.eax.readUtf8String();
        send(output);
    },

  });
    """
# for i in range(5):
#     script=process.create_script(
#         source
#     )
#     script.on('message', on_message)
#     script.load()
#     time.sleep(1)
#     script.unload()

def run_script():
    script = process.create_script(source)
    script.on('message', on_message)
    script.load()
    time.sleep(1)
    script.unload()

for i in range(20):
    thread = threading.Thread(target=run_script)
    thread.start()
    time.sleep(1)