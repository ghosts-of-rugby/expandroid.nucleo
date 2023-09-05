# Expandroid.Nucleo

<div align="center"><img src="./nucleo144.webp" height=200/></div>

[CATCHROBO](https://catchrobo.net/)でロボットを制御するための Nucleo ボードのプロジェクト。

ラップトップ PC<=>Nucleo は Ethernet(UDP)で通信し，Nucleo<=>ロボットは CAN で通信，ラップトップとロボットを中継する役割を担う。

Nucleo ボードとしては[Nucleo-144 STM32F767ZI](https://www.st.com/ja/evaluation-tools/nucleo-f767zi.html)を使用する。

## Installations for Linux

### STM32CubeMX

### toolchain

### [STLINK Tools](https://github.com/stlink-org/stlink)

```bash
sudo apt install stlink-tools
```

以下のコマンドで書き込みを行う。

```bash
st-flash write build/Expandroid.Nucleo.bin 0x8000000
```

### VSCode でのデバッグ環境

## Configations

### Network

| Name         | Value           |
| ------------ | --------------- |
| NUCLEO IP    | 192.168.137.131 |
| PORT for LED |                 |

### User Button

```json
{
  "time": 1000
}
```

### LED

|     |            |
| --- | ---------- |
| LD1 | Unable     |
| LD2 | System LED |
| LD3 | Enable     |

<!-- ### CAN

- Start/Stop

  ```json
  {
    "action": "start",
    "filter": {
      "id": 0x000,
      "mask": 0x000,
      "fifo": 0,
      "bank": 0,
      "mode": "mask", // mask or list
      "scale": "16bit" // 16bit or 32bit
    }
  }
  ```

  ```json
  {
    "action": "stop"
  }
  ```

- TX Data

  ```json
  {
    "action": "send",
    "header": {
      "id": 123,
      "is_extended_id": false,
      "is_remote_frame": false,
      "dlc": 8
    },
    "message": [1, 2, 3, 4, 5, 6, 7, 8]
  }
  ``` -->

### RoboMaster Motor

- Add Motor

  ```json
  {
    "act": "add",
    "id": 1
  }
  ```

- Set Paramenters

  ```json
  {
    "act": "set",
    "params": [
      {
        "id": 1,
        "name": "cmd",
        "val": "c"
      },
      {
        "id": 1,
        "name": "kp",
        "val": 1.0
      }
    ]
  }
  ```

- Get Parameters

  ```json
  {
    "act": "get",
    "params": [
      {
        "id": 1,
        "name": "cmd"
      },
      {
        "id": 1,
        "name": "kp"
      }
    ]
  }
  ```
