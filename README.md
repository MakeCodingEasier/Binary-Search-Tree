运行前确保当前命令行为UTF-8编码，例如：对于 PowerShell，可以使用如下命令设置输出编码为UTF - 8。

```powershell
[Console]::OutputEncoding = [System.Text.UTF8Encoding]::new()
```

再依次运行以下命令：

```powershell
gcc -std=c99 -o main main.c B_Tree.c RB_Tree.c Tree.h -fexec-charset=utf-8
./main
```