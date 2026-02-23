# import asyncio
# import websockets
# from typing import Set

# class MinimalWebSocketServer:
#     """最小化的 WebSocket 服务器（仅核心功能）"""
    
#     def __init__(self, host="0.0.0.0", port=9000):
#         self.host = host
#         self.port = port
#         self.clients = set()
    
#     async def echo_server(self, websocket, path):
#         """简单的回显服务器"""
#         self.clients.add(websocket)
#         print(f"新客户端连接: {websocket.remote_address}, 路径: {path}")
#         try:
#             async for message in websocket:
#                 print(f"收到消息: {message}")
#                 # 向所有客户端广播
#                 for client in self.clients:
#                     if client != websocket:
#                         await client.send(f"trans: {message}")
#                 await websocket.send(f"received : {message}")
#         finally:
#             self.clients.remove(websocket)
#             print(f"客户端断开: {websocket.remote_address}")
    
#     async def start(self):
#         # 创建一个包装函数，正确绑定self
#         async def handler(websocket):
#             await self.echo_server(websocket, websocket.request.path)
        
#         server = await websockets.serve(
#             handler,
#             self.host,
#             self.port
#         )
#         print(f"简易 WebSocket 服务器启动: ws://{self.host}:{self.port}")
#         await server.wait_closed()

# if __name__ == "__main__":
#     server = MinimalWebSocketServer(port=9000)
#     asyncio.run(server.start())

import asyncio
import websockets
from typing import Set
import logging

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

class RobustWebSocketServer:
    """增强的 WebSocket 服务器"""
    
    def __init__(self, host="0.0.0.0", port=9000, ping_interval=20, ping_timeout=10):
        self.host = host
        self.port = port
        self.clients = set()
        self.ping_interval = ping_interval
        self.ping_timeout = ping_timeout
    
    async def echo_server(self, websocket, path):
        """带有错误处理的回显服务器"""
        self.clients.add(websocket)
        client_addr = websocket.remote_address
        logger.info(f"新客户端连接: {client_addr}, 路径: {path}")
        
        try:
            async for message in websocket:
                logger.info(f"收到来自 {client_addr} 的消息: {message}")
                
                # 向所有其他客户端广播
                disconnected_clients = set()
                for client in self.clients:
                    if client != websocket:
                        try:
                            await client.send(f"trans: {message}")
                        except websockets.exceptions.ConnectionClosed:
                            logger.warning(f"客户端 {client.remote_address} 连接已关闭，移除")
                            disconnected_clients.add(client)
                        except Exception as e:
                            logger.error(f"发送到 {client.remote_address} 失败: {e}")
                            disconnected_clients.add(client)
                
                # 清理已断开的连接
                for client in disconnected_clients:
                    if client in self.clients:
                        self.clients.remove(client)
                        logger.info(f"已移除断开连接: {client.remote_address}")
                
                # 回复发送者
                await websocket.send(f"received: {message}")
                
        except websockets.exceptions.ConnectionClosed as e:
            logger.info(f"客户端 {client_addr} 正常断开: {e.code}")
        except asyncio.exceptions.TimeoutError as e:
            logger.error(f"客户端 {client_addr} 连接超时: {e}")
        except Exception as e:
            logger.error(f"客户端 {client_addr} 发生错误: {e}")
        finally:
            if websocket in self.clients:
                self.clients.remove(websocket)
            logger.info(f"客户端 {client_addr} 已移除, 当前连接数: {len(self.clients)}")
    
    async def start(self):
        """启动服务器"""
        async def handler(websocket):
            await self.echo_server(websocket, websocket.request.path)
        
        # 设置 ping_interval 和 ping_timeout 来保持连接活跃
        server = await websockets.serve(
            handler,
            self.host,
            self.port,
            ping_interval=self.ping_interval,
            ping_timeout=self.ping_timeout
        )
        
        logger.info(f"WebSocket 服务器启动成功: ws://{self.host}:{self.port}")
        logger.info(f"心跳设置: ping_interval={self.ping_interval}s, ping_timeout={self.ping_timeout}s")
        
        # 保持服务器运行
        await server.wait_closed()
    
    async def close_all(self):
        """关闭所有客户端连接"""
        for client in self.clients.copy():
            if client.open:
                await client.close()
        logger.info(f"已关闭所有 {len(self.clients)} 个连接")

if __name__ == "__main__":
    # 创建并启动服务器
    server = RobustWebSocketServer(
        port=9000,
        ping_interval=20,  # 20秒发送一次ping
        ping_timeout=10    # 10秒内没收到pong则认为超时
    )
    
    try:
        asyncio.run(server.start())
    except KeyboardInterrupt:
        logger.info("服务器正在关闭...")
    except Exception as e:
        logger.error(f"服务器异常: {e}")