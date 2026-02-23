import asyncio
import websockets

async def echo_client():
    # 连接到你的服务器
    async with websockets.connect("ws://192.168.3.12:9000/test") as ws:
        print("已连接到服务器")
        
        while True:
            # 发送用户输入
            message = input("输入消息 (输入quit退出): ")
            if message.lower() == "quit":
                break
            
            await ws.send(message)
            print(f"发送: {message}")
            
            # 接收服务器响应
            response = await ws.recv()
            print(f"服务器回复: {response}")

# 运行
asyncio.run(echo_client())