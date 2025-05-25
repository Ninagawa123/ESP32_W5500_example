#!/usr/bin/env python3
import socket
import threading
import time

# UDP設定
UDP_RECEIVE_PORT = 22222  # ESP32からの受信ポート
UDP_SEND_PORT = 22224     # ESP32への送信ポート

# ネットワーク設定（適宜変更してください）
MAC_IP = "192.168.90.1"       # MacのIP（有線LAN）
ESP32_IP = "192.168.90.2"    # ESP32のIP

# 送信メッセージ（変更可能）
SEND_MESSAGE = "Hello, This is Mac. Are you ESP32?"

class UDPServer:
    def __init__(self):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind((MAC_IP, UDP_RECEIVE_PORT))
        self.running = True
        
    def receive_messages(self):
        """ESP32からのメッセージを受信"""
        print(f"UDP Server listening on {MAC_IP}:{UDP_RECEIVE_PORT}")
        
        while self.running:
            try:
                data, addr = self.sock.recvfrom(1024)
                message = data.decode('utf-8')
                print(f"Received from {addr}: {message}")
            except socket.timeout:
                continue
            except Exception as e:
                print(f"Receive error: {e}")
                
    def send_messages(self):
        """ESP32へメッセージを送信（2秒間隔）"""
        send_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        
        while self.running:
            try:
                send_sock.sendto(SEND_MESSAGE.encode('utf-8'), (ESP32_IP, UDP_SEND_PORT))
                print(f"Sent to {ESP32_IP}:{UDP_SEND_PORT}: {SEND_MESSAGE}")
                time.sleep(2)  # 2秒間隔
            except Exception as e:
                print(f"Send error: {e}")
                time.sleep(2)
                
    def start(self):
        """サーバー開始"""
        print("Starting UDP Server...")
        print(f"Mac IP: {MAC_IP}")
        print(f"ESP32 IP: {ESP32_IP}")
        print("Press Ctrl+C to stop")
        
        # 受信スレッド開始
        receive_thread = threading.Thread(target=self.receive_messages)
        receive_thread.daemon = True
        receive_thread.start()
        
        # 送信スレッド開始
        send_thread = threading.Thread(target=self.send_messages)
        send_thread.daemon = True
        send_thread.start()
        
        try:
            while True:
                time.sleep(1)
        except KeyboardInterrupt:
            print("\nShutting down...")
            self.running = False
            self.sock.close()

if __name__ == "__main__":
    # 使用方法とIPアドレス確認
    print("=== ESP32 ⇔ Mac UDP Communication Test ===")
    print(f"Mac IP (有線LAN): {MAC_IP}")
    print(f"ESP32 IP: {ESP32_IP}")
    print("\n※ IPアドレスを環境に合わせて変更してください")
    print("※ ifconfig コマンドでMacの有線LANのIPを確認できます")
    print()
    
    server = UDPServer()
    server.start()