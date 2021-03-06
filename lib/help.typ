*?1*
                        Typistへようこそ.

Typist は、キーボードのタッチタイピング（ブラインドタッチ）を学習するた
めのソフトウェアです。タッチタイピングの基本から少しずつ練習を積み重ね、
やがてタイピングのエキスパートになるまでのカリキュラムが用意されています。

これまでにタイプの練習をしたことがない人でも、心配は無用です。タイピング
は、練習にこそ多くの時間を必要としますが、難しいものではないのです。
ただし、タイプする時にキーの上の指を見ることは避けましょう。これは大変悪
い習慣で、なかなか上達できません。このプログラムでは、誤ったキーインは画
面上でそれとわかるように表示します。

あなたがこれまで、目で見てキーを押す方法でやって来たのなら、見ないで打つ
ことに不安を感じるかもしれません。初めのうちは、これまでの方法よりも遅い
かもしれません。でも、一度この方法を身につけてしまえば、結局は格段に速く
入力できるようになります。\T
                        日本語入力について

Typistでは英文タイプはもちろん、ローマ字かな入力、さらに、かな入力による
タイプの練習ができます。

Typistのカリキュラムはもともと海外で開発されたものですので、英文タイプの
練習が多くなっています。このレッスンは長年世界中で利用されている、定評の
あるものです。
Typistでは、日本語のタイプ練習でも、タイピングにおいて重要な指の位置や指
の動かし方が身に付くようにレッスン内容を工夫してあります。

日本語のタイプ練習は、特定の日本語入力システムに依存しないように構成され
ています。\T
                        対象とするキーボード

これまでにワープロやタイプライタ、パソコンなどを扱ったことのある人なら、
キーボードと言っても文字の配列の仕方にさまざまなものがあることをご存じで
しょう。

Typistは、標準のアルファベット入力の上達にもっとも力を注いだカリキュラム
を提供しています。従って、このカリキュラムに従って学習すれば、多少異なっ
たキーボードにもすぐに適用できます。
Typingではさまざまなパソコン環境で実行できるように、標準的な日本語、英語
キーボードのキー配列で無理なく練習できることを確認し、また、多少の改良を
加えています。

もちろん、色々な記号の入力をタッチタイピングで行いたい人のために、記号の
入力用のレッスンも用意しています。また、いわゆるテンキーによる数値入力の
ために、テンキーのタッチタイピングのコースも用意してあります。\T
                        キーボードの設定

#if e
現在、101キーボードの設定で動作しています。

このキーボードでは、改行キーの近くに '[' と ']' が隣り合って配置され、数
字の '2' とシフトキーで '@' が入力できるはずです。
#elif j
現在、106キーボードの設定で動作しています。

このキーボードでは、改行キーの近くに '[' と ']' が縦に並んで配置され、数
字の '2' とシフトキーで '"' が入力できるはずです。
#else
現在、101キーボード、106キーボード以外のキーボード設定で動作しています。

#endif
正しく入力できない場合、キーボードのタイプを -k オプションで指定して、
Typistを起動し直す必要があります。

        -kj     106キーボード
        -ke     101キーボード

#if j
例えば、101キーボードの設定にするには、次のようなコマンドで起動します。
	typist -ke
#else
例えば、106キーボードの設定にするには、次のようなコマンドで起動します。
	typist -kj
#endif
この設定は一回行えば、設定用ファイルに記録されますので、次回からは指定す
る必要はありません。

今すぐ Typist を終了したい場合には、Control-C を入力して下さい。\T
                        どのコースを選択すべきか

Typingにはさまざまな練習用のコースが用意されています。どれを選択して練習
したらよいのか、戸惑ってしまうかもしれませんね。

    ローマ字入力コース (a1 - a20)     ローマ字実践コース (b1 - b14)
    かな入力コース     (k1 - k17)     かな実践コース     (j1 - j14)
    英文標準コース     (t1 - t16)     英文スピードアップ (s1 - s4)
    英文標準ドリル     (u1 - u13)     英文速習コース     (q1 - q5)
    英文集中ドリル     (v1 - v20)     英文上達コース     (r1 - r14)
    英文実戦ドリル     (m1 - m11)     テンキー入力コース (n1 - n3)

ローマ字かな入力には「ローマ字入力コース」(Aコース)を選択します。これだ
けでも英文は入力できるようになりますが、大文字や記号の入力方法をマスター
しなければ英文をすらすら入力はできません。
英文タイプの練習には「標準コース」(Tコース)を選択すれば十分です。簡単に
英文タイピングの概要を身につけたい人は、代わりに「速習コース」を選択する
こともできます。\T
                        練習の仕方

練習の方法は、レッスンの中でも説明されています。ここでは概略だけを説明し
ておきましょう。

レッスンでは、画面の中ほどにテキストを１行〜数行表示し、画面の上の方に指
示を表示します。
あなたがすべきことは、画面の中ほどに表示されたテキストの文字をタイプする
ことです。正しい文字を入力した時には特に何もしません。誤った場合には、大
文字のXを表示し、音を出します。正しくても間違っていても、とにかくタイプを
続けて下さい。Deleteキーは使えません。
行末まで打ち終えたら、改行(Return)キーを打ちます。

その画面を誤りなくタイプできたら、次の練習用パターンが表示されます。間違
えてタイプした場合には、音で知らせ、同じ画面をもう一度やりなおしてもらう
ことになります。\T
実際にやってみましょう。Fのキーを左人差し指で４回打って下さい。\I
ffff\D
                        スピード練習

練習の方法には、もうひとつ、スピード練習と呼んでいるものがあります。
前の練習では、表示されたテキストの下の行にタイプして行きましたが、この方
法では、表示されたテキストの上にタイプして行きます。

タイプ間違いをした時には、音で知らせ、誤った部分を強調表示します。Delete
キーを使うこともできますが、できれば使わないでどんどん打ち続けて下さい。

この練習では、どのくらい早く正確に入力できたか、を自己採点します。タイプ
し終わったら、画面の下に毎分に入力できる打鍵数、および単語数(語/分:WPM)
という形で入力スピードが計算されます。また、誤りの数も数えて、総数を表示
します。\T
実際にやってみましょう。Jは右人差し指、空白は右親指で打ちます。\I
fff jjj fjfj\P
                        ローマ字かな入力

Typistでは日本語入力システム（WindowsのIMEやATOK、ことえりなど）を利用せ
ずに、ローマ字かな入力、かな入力の練習ができます。これらのレッスンでは、
日本語入力システムを起動しないで練習して下さい。画面にはひらがなが表示さ
れます。\T
ローマ字かな入力をやってみましょう。「い」は i、「き」は ki です。\I
いいい　ききき\K
                        練習履歴データ

Typistで練習した履歴、つまり、どのコースはどのレッスンまで終わったかとい
う情報がファイルの上に記憶されます。
このファイルに書かれた情報は次回に練習を開始する時にメニューといっしょに
表示されますので、新しいレッスンから練習を開始できます。

UNIXの場合は ~/.typistrc というファイルに、MS-DOSやWindowsでは HOME
または HOMEPATH という環境変数が表すディレクトリ内の typist.dat という
ファイルに記録されます。
ただし、環境変数 TYPDATA にファイルのパスが設定されていれば、そのファイル
が履歴ファイルとなります。\T
                        練習中の特殊なキー操作

練習の画面が表示されている時、以下のキーで特別な動作が可能です。

        Control-F       その画面の練習をせずに次の画面に移ります。

        Control-L       その時に選択しているレッスンを中止して、
                        コース選択の画面に戻ります。

        Control-C       Typist を終わりにします。

ただし、あるレッスンの中で Control-F を使うと、そのレッスンは真面目に練習
して終了したものとは見なされませんので注意して下さい。
逆に、この機能はレッスンの内容をざっと見るのに便利です。\T
                        タイプ後の特殊なキー操作

画面の一番下に「次に進むには何かキーを押して下さい...」と表示されている時
には、次のキーは特殊な機能キーとして働きます。

        Backspaceキー   あるいは
        Control-H       その画面の練習内容をもう一度繰り返します。
                        （練習の後にだけ有効）

        Control-G       間違えた時のクリック音（ビープ音）を出す／出さない
                        の切り替えを行います。

        ?               簡単なヘルプを表示します。

打ち間違いの有無に関わらず、ある画面の表示内容を何回も繰り返して練習したい
場合には、Backspaceキーで繰り返し練習が可能です。
通常は１回しか練習できないスピード練習にも、何度も挑戦できます。\T
                        コマンドライン

  Typistを起動する時、コマンドラインにオプションを指定できます。

一般形:  typist [オプション] [lesson]

オプション:
        -k?     ? でキーボードのタイプを指定します。
                j: 106キー,  e: 101キー
        -L???   ??? で示されるディレクトリのレッスンデータを使います。
                http:で始まる文字列で、Web上のファイルを指定できます。
        -q      間違えた時にクリック音（ビープ音）を出さないようにします。
        -V      現在のバージョン情報を表示します。

Lesson:
        練習したいレッスンを直接指定します。例えば
                typist t5
        とすると、レッスン t5 をすぐに始められます。\T
CopyRight

  Typist は DECUSのテープのBASIC版を元に、D. Jason Penney氏によって作成さ
れ、配布されました。この日本語版の元となったものは、uxc.cso.uiuc.edu で配
布されていた 1991年11月の版です。

  プログラムの日本語への対応、および機能の拡張は荻原によって行われました。
  レッスン用データの日本語化および内容の誤りの訂正、一部変更はすべて荻原の
責任において行われております。
  なお、Typistと同様のタイプ練習を行うプログラムに gtypist (GNU Typist)
がありますが、本プログラムはgtypistとはまったく別に開発されたものであり、
GPLには一切抵触しません。
  このプログラムとレッスン用データは自由に再配布可能ですが、有償での販売等
はお断りします。日本語部分に荻原の著作権が存在することを明記しておきます。

                             2007-08-23    荻原剛志 (t_ogihara@mac.com)\T
*
