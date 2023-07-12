# Expandroid.Nucleo

CATCHROBO(https://catchrobo.net/)でロボットを制御するためのNucleoボードのプロジェクト。

ラップトップPC<=>NucleoはEthernetで通信し，Nucleo<=>ロボットはCANで通信，ラップトップとロボットを中継する役割を担う。

NucleoボードとしてはSTM32F767ZI(https://www.st.com/ja/evaluation-tools/nucleo-f767zi.html)を使用する。

![nucleo](./nucleo144.webp)

## Installations

Linuxのみ対応。

### STM32CubeMX

CubeMXの使い方を説明する。

### toolchain

ソースコードをビルドするためにはツールチェインが必要．
toolchainはCubeCLTに含まれているため，CubeCLTのインストールの方法を説明する。

### STLINK Tools

https://github.com/stlink-org/stlink

```bash
sudo apt install stlink-tools
```

以下のコマンドで書き込みを行う。

```bash
st-flash write build/Expandroid.Nucleo.bin 0x8000000
```

### VSCodeでのデバッグ環境

## Configations

### サーバーとクライアントのわけ方

CANの送信，GPIOの読み込み，LEDの制御はPCから指令が来て，それに従いマイコン側が動作するため，サーバーとして動作する．
一方，CANの受信割り込みや，GPIOの割り込みはマイコン側から指令が来て，それに従いPC側が動作するため，クライアントとして動作する．

### Network

| Name    | Value         |
| ------- | ------------- |
| IP      | 192.168.137.1 |
| Mask    | 255.255.255.0 |
| Gateway | 192.168.137.1 |

### User Button

```json
{
  "time": 1000,
}
```

### LED

LED2は通信状態を表す．