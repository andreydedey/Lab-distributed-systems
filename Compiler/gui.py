import tkinter as tk
from tkinter import scrolledtext, filedialog, messagebox
import socket

SERVER_HOST = 'localhost'
SERVER_PORT = 5000

def enviar_codigo():
    codigo = editor.get("1.0", tk.END)
    saida.delete("1.0", tk.END)
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(10)  # timeout para evitar travamento
        s.connect((SERVER_HOST, SERVER_PORT))
        s.sendall(codigo.encode())
        resposta = b""
        while True:
            try:
                data = s.recv(4096)
                if not data:
                    break
                resposta += data
            except socket.timeout:
                break
        s.close()
        saida.insert(tk.END, resposta.decode(errors="replace"))
    except Exception as e:
        messagebox.showerror("Erro", f"Falha ao conectar ou enviar código:\n{e}")

def baixar_codigo():
    codigo = editor.get("1.0", tk.END)
    filename = filedialog.asksaveasfilename(defaultextension=".pl", filetypes=[("Prolog files", "*.pl"), ("All files", "*.*")])
    if filename:
        try:
            with open(filename, "w") as f:
                f.write(codigo)
            messagebox.showinfo("Sucesso", f"Código salvo em:\n{filename}")
        except Exception as e:
            messagebox.showerror("Erro", f"Falha ao salvar arquivo:\n{e}")

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