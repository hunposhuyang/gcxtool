# gcxtool
合金装备4中，stage/init/scenerio.gcx这个文件中包含游戏里的界面文本，为了本地化这些文本，我需要将里面的字符串解密并导出，这个工具就是为此而生。
这个程序有以下的功能：
- 从scenerio.gcx中解密并导出stringData部分为bin文件，路径为程序目录/gcxFilename_strres/scenario.bin
- 从scenerio.gcx和bin文件合并一个scenerio.gcx,路径为程序目录/gcxFilename_strres/scenario.gcx
- 支持psn版游戏加密的scenerio.gcx
本程序只适用于以上所说的位于stage/init里面的scenerio.gcx文件，不能解析打包其他gcx文件。
这个项目的大部分代码来自于Solideye by Jayveer，Gcx by Jayveer
## 构建
```
clang bin.cpp Blowfish.cpp crypto.cpp decryptor.cpp gcx.cpp md5.cpp main.cpp -o gcxtool.exe -std=c++17
```

## 使用方法
- 导出bin文件
```
gcxtool -e path/to/scenerio.gcx
```
- 合并为gcx文件
```
gcxtool -c path/to/scenerio.gcx path/to/binfile
```
-对于psn版的加密的gcx文件
```
gcxtool -c path/to/scenerio.gcx path/to/binfile -k key
```
```
gcxtool -e path/to/scenerio.gcx -k key
```
key是gcx文件相对于stage文件夹的路径，如：
```
gcxtool -e path/to/scenerio.gcx -k stage/init
```

## 许可证
本项目所有内容适用 MIT 许可证
