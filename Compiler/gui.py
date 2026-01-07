import tkinter as tk
from tkinter import scrolledtext, filedialog, messagebox
import socket

SERVER_HOST = 'localhost'
SERVER_PORT = 5000

def enviar_codigo():
    codigo = editor.get("1.0", tk.END)
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((SERVER_HOST, SERVER_PORT))
        s.sendall(codigo.encode())
        resposta = b""
        while True:
            data = s.recv(4096)
            if not data:
                break
            resposta += data
        s.close()
        saida.delete("1.0", tk.END)
        saida.insert(tk.END, resposta.decode(errors="replace"))
    except Exception as e:
        messagebox.showerror("Erro", str(e))

def baixar_codigo():
    codigo = editor.get("1.0", tk.END)
    filename = filedialog.asksaveasfilename(defaultextension=".pl", filetypes=[("Prolog files", "*.pl"), ("All files", "*.*")])
    if filename:
        with open(filename, "w") as f:
            f.write(codigo)

root = tk.Tk()
root.title("Cliente Prolog - Laboratório de Sistemas Distribuídos")

tk.Label(root, text="Código PROLOG:").pack()
editor = scrolledtext.ScrolledText(root, width=80, height=20)
editor.pack()

btn_frame = tk.Frame(root)
btn_frame.pack(pady=5)
tk.Button(btn_frame, text="Executar Código", command=enviar_codigo).pack(side=tk.LEFT, padx=5)
tk.Button(btn_frame, text="Baixar Código", command=baixar_codigo).pack(side=tk.LEFT, padx=5)

tk.Label(root, text="Saída/Erros do Servidor:").pack()
saida = scrolledtext.ScrolledText(root, width=80, height=10)
saida.pack()

root.mainloop()