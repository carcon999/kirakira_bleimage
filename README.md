# kirakira_bleimage
You can create an afterimage on the website and send it directly.

![Image](https://github.com/carcon999/kirakira_bleimage/blob/master/img/helloworld.png)

# 概要
このソフトウェアは、[micro:bit](https://microbit.org/)に40連LEDバー（[kirakira:bit](https://www.switch-science.com/catalog/3923/)）を取り付け、本体を左右に振るとお絵かきした絵が残像現象で見えるプログラムです。ブラウザ（Chrome限定）でお絵かきしたデータを無線（BLE）経由で送ることができます。ただし、残念ながらiPhone,iPadのChromeでは動作しませんのでご注意ください。Windows/Mac/Androidではおおむね動作しますが、動作保障するものではありません。

![Image](https://github.com/carcon999/kirakira_bleimage/blob/master/img/overview.png)

# 必要なもの
|名称|説明|備考|
|:---|:---|:---|
|[micro:bit](https://microbit.org/)|マイクロビット|本体|
|[kirakira:bit](https://www.switch-science.com/catalog/3923/)|LEDバー|40個の小型LED搭載|
|電源|PHコネクタ経由で3.7-5V|[リチウムイオンポリマー電池](https://www.switch-science.com/catalog/3118/)がBEST|
|パソコン|ビルド用|Arduino IDEが動作するパソコン|
|microUSBケーブル|プログラム書き込み用||

# プログラムの書き込み
開発には、Arduino IDEが必要となります。Arduino IDEが動作するパソコンであればプログラムを書き込むことができます。ただし、一部のライブラリを修正する必要があるので順番に実施してください。

最初に既に公開済みの開発環境説明で基本的な環境を構築（1.）した上で今回の環境を構築します。ですので、既にkirakira:bit用の開発環境を構築済みの場合はスキップして2.の手順を実施してください。

1. こちらの[kirabitDemo](https://github.com/carcon999/kirabitDemo/blob/master/README.md)のプログラムを手順に従って環境構築(書き込み)してください。

2. [こちら](https://github.com/carcon999/kirakira_bleimage/blob/master/Environment.md)が、今回のプログラム環境構築手順になります。少し長いのですが順番に実施してください。

# 動作確認と各種操作

1. [kirakira:bit](https://www.switch-science.com/catalog/3923/)側へ電源を供給します。コネクタがPHコネクタとなっておりますので、小型のリチウムイオンポリマー電池が適合します。※kirakira:bitには充電機能はありません。実は、PHコネクタはmicro:bit側にも存在しますが、間違ってmicro:bit側へ高い電圧を供給すると壊れる場合があるのでご注意ください。
![Image](https://github.com/carcon999/kirakira_bleimage/blob/master/img/power.jpg)

2. 電源のスライドSWをONにし、本体を左右に振ると各国の旗が次々と残像で見えれば、成功です。デフォルトでは11種類の国旗が組み込まれています。[こちら](https://hpgpixer.jp/image_icons/flags.html)のサイトのデータを利用させていただきました。
![Image](https://github.com/carcon999/kirakira_bleimage/blob/master/img/flag.png)

3. micro:bitのAボタンを押下すると現在表示中の画像でロックします。再度Aボタンを押下するとロックが解除されます。お気に入りの画像だけを表示したい場合にロックできます。

4. micro:bitのBボタンを短押しすると、本体を振ること無しに連続してLEDを高速点灯します。再度短押しすると解除します。テスト等でご利用ください。

5. micro:bitのBボタンを長押しすると、Bluetooth(BLE)の通信モードになります。BLEモード中は、micro:bit本体の5x5マトリックス表示で通信状態を表示します。※BLEモード中は、LEDバーは点灯しません。通信モードの解除は、Aボタンを短押しすると解除されます。
![Image](https://github.com/carcon999/kirakira_bleimage/blob/master/img/blemode.png)

# BLEモードでの画像転送方法

1. 表示したい画像を準備します。[こちら](https://s3-ap-northeast-1.amazonaws.com/microbitble/imagecanvas.html)でお絵かきとそのデータを送ることができます。Chromeのブラウザでのみ動作します。Windows/Mac/Androidで動作確認しています。残念ながらiPhone/iPadは動作しませんのでご注意ください。
1. お絵かきが完了したら、micro:bitのBボタンを長押し、BLEモードにします。内部の状態としては、相手側からの接続待ちの状態となります。
1. Webサイトの「データ送信」ボタンを押下します。ペア設定を要求するダイアログが表示されますので、「kirakirabit」を選択し、「ペア設定」ボタンを押下します。
1. Webサイトのプログレスバーが転送状態を表示し、micro:bitの5x5LEDもドットで進捗状態を表示します。
1. 100%になることで、転送が完了しBLEモードも自動的に解除されます。本体を左右に振るとお絵かきした画像が表示されます。
　※もし、上手くできない場合は、micro:bit側の電源を一度OFF->ONしてから、2.の手順からやり直してください。

以上



