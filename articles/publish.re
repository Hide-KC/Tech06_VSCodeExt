={publish} 拡張機能のインストールとマーケットプレイスへの公開

この章では、作成した拡張機能を VSIX ファイルにコンパイルし、VSCode へオフラインでインストールを行います。
また、マーケットプレイスに公開し、世界に向けて発信を行いたいと思います。
ちなみに、題材として @<chap>{preparation} で生成した Hello World を使用し説明を行っていきます。
公開まで Hello World を使いますが、マーケットプレイスを検索すると
みんなオリジナル Hello World を公開しているので、安心して公開しちゃいましょう。

=={azure} Azure DevOps の登録

作成した拡張機能をコンパイル、または公開するときは、 @<code>{publisher} という発行者の情報が必要になります。
@<code>{publisher} の登録は Azure DevOps @<fn>{azure_devops_cite} でできるので、
とりあえずユーザ登録を済ませてしまいましょう。

//footnote[azure_devops_cite][Azure DevOps https://azure.microsoft.com/ja-jp/services/devops/]

Azure DevOps にサインインしたら、 @<img>{azure_cite} の「無料で始める」をクリックします。

//image[azure_cite][Azure DevOps サイト][scale=0.72]{
//}

すると最初にプロジェクトの管理画面が表示されます。右上のユーザのアイコンから Security をクリックします。

//image[azure_humburg][アイコンからSecurityをクリック][scale=0.72]{
//}

Security の中から Personal access tokens を選択し、New Token をクリックします。

//image[azure_tokens][Personal access tokens][scale=1.0]{
//}

必要な項目を入力します。

 * Expiration
 ** トークンの有効期間を決めます。最長で1年間まで有効にできます。
 * Scopes
 ** Custom defined
 ** Marketplace を探し、Read/Acquire/Publish/Manage 全てにチェック

Create を押すと、トークンが発行されます。
このトークンは@<b>{このとき一度きりしか見られません。}このあとの @<code>{publisher} の登録時に
必要となるので、必ずコピペしておきましょう。もしメモを忘れてしまった場合は、新しくトークンを生成してくさい。

//image[azure_create][トークンの作成][scale=0.72]{
//}

トークンをメモしたら Azure DevOps は閉じてしまってかまいません。
次に進みましょう。

== VSIX ファイル

拡張機能をオフラインでインストールするときは、 VSIX（@<code>{.vsix}） というファイルが必要になります。 @<fn>{urawaza}
VSIX は拡張機能をコンパイルしたファイルで、後述の @<code>{vsce} モジュールにより生成します。

//footnote[urawaza][~/user/.vscode/extensions にプロジェクトフォルダをコピペしても一応使えるようにはなるそうです……正常に動くのかはわかりません(´・ω・`)]

== vsce モジュールのインストール

@<code>{vsce} は Visual Studio Code Extensions を縮めたもので、
拡張機能のパッケージングやマーケットプレイスへの公開、拡張機能の管理を行う @<code>{npm} モジュールです。
次のコマンドでインストールします。

//cmd{
$ npm install -g vsce
$ vsce -V
1.56.0
//}

バージョンが返ってくれば OK です。 @<fn>{vsce_help}

続いて、 @<hd>{azure} で取得したトークンを使って @<code>{publisher} を登録します。

//cmd{
$ vsce create-publisher "kcpoipoi"
...
Token [Tokenをコピペ]
...
# 登録されている publisher 一覧を表示する
$ vsce ls-publishers
kcpoipoi
//}

登録した @<code>{publisher} が一覧で返ってくれば OK です。
これを、 Hello World の @<code>{package.json} に記述します。

//list[hello_package][Hello World - package.json]{
{
  "publisher": "kcpoipoi"
}
//}

これで発行の準備が整いました。

//footnote[vsce_help][ちなみに @<code>{$ vsce} だけでヘルプが表示されます]

=={generate_vsix} VSIX ファイルの生成

それでは作成したプロジェクトを VSIX ファイルにコンパイルしてみましょう。
拡張機能の @<code>{package.json} があるフォルダに移動して、次のコマンドでコンパイルを行います。

//cmd{
$ cd ~/helloworld
$ vsce package
# helloworld.vsix generated
//}

ここで、コマンド系拡張機能を TypeScript で作っていたりすると、次のような原因でコンパイルに失敗するかもしれません。
筆者が（VSIX 生成時に）踏み抜いた地雷はこれだけですが、他にも原因があればめげずに挑戦してみましょう。

==== 「問題」がある

@<code>{Ctrl+Shift+M} で問題タブを見てみます。

//image[problem][問題][scale=1.0]{
//}

この状態で @<code>{$　vsce　package} を実行すると、次のエラーでこけてしまいます。

//cmd{
ERROR Command failed: npm run vscode:prepublish
//}

@<chap>{preparation} で用意した Hello World は、TypeScript により型チェックを厳しく行うよう設定していました。
この場合、どこからも参照されていないメソッドが存在するため TypeScript コンパイラに弾かれてしまいます。
参照されていないメソッドを消すか、型チェックを緩めてコンパイルが通るようにしましょう。 @<fn>{tslint_cite}

//list[tslint][tslint.json]{
{
  "strict": true,   /* enable all strict type-checking options */
  //これをfalseにして、各strictオプションを個別に設定する
}
//}

//footnote[tslint_cite][tslint のオプション設定はこちら https://www.typescriptlang.org/docs/handbook/compiler-options.html]

==== README.md がデフォルトのまま

作成した拡張機能フォルダ内の @<code>{README.md} がデフォルトのままだと次のエラーでこけます。

//cmd{
ERROR  Make sure to edit the README.md file before you publish your extension.
//}

@<code>{README.md} は拡張機能の「詳細」タブの部分に対応します。
本文を消したり書き換えたりして修正しましょう。

//image[extension_detail][README.mdに記述した内容は詳細タブに表示される][scale=1.0]{
//}

また、Warning として次のようなメッセージが表示されるかもしれません。

//cmd{
WARNING  A 'repository' field is missing from the
'package.json' manifest file.Do you want to continue? [y/N] y
//}

@<code>{package.json} にリポジトリがないけど大丈夫？と言われていますが、サンプルなので Yes でいいでしょう。
これらをクリアし、次のメッセージが表示されれば VSIX ファイルの出力完了です。

//cmd{
DONE  Packaged: ~\helloworld\helloworld-0.0.1.vsix (377 files, 728.79KB)
//}

== VSIX ファイルでインストール

無事に VSIX ファイルが出力されたら、次はインストールしてみましょう。
VSIX ファイルからのインストールには２つの方法があります。

==== GUI でインストール

「VSIX からのインストール」をクリックし、 @<code>{.vsix} ファイルを選択すれば OK です。

//image[vsix_install][GUIで拡張機能をインストール][scale=0.72]{
//}

==== コマンドでインストール

VSIX のあるフォルダに移動し、次のコマンドでインストールします。

//cmd{
$ code --install-extension helloworld.vsix
//}

念のため VSCode を再起動します。これで拡張機能が使える！！

//image[install_hello][インストールすると拡張機能の一覧に表示される][scale=1.0]{
//}

@<b>{───そう思っていた時期が私にもありました。}

==== npm install の実行

再起動した VSCode で、 @<code>{HelloWorld} コマンドを実行してみましょう。
すると、もしかしたら次のようなエラーメッセージが表示されるかもしれません。 @<fn>{you_are_lucky}

//footnote[you_are_lucky][普通に実行できればあなたはラッキーです(ﾏﾃ]

//image[command_failed][caption][scale=1.0]{
//}

原因は、次のように @<code>{tsconfig.json} に記述されており、
VSIX ファイルに @<code>{node} モジュールが含まれないためです。 @<fn>{module_issue}
VSIX からのインストール特有の問題のようです。

//footnote[module_issue][過去にIssue上がっている経緯もある https://github.com/Microsoft/vscode/issues/15837]

//list[exclude_node][Hello World - tsconfig.json]{
{
  "exclude": [
    "node_modules",
    ".vscode-test"
  ]
}
//}

そこで、拡張機能をインストールしたフォルダに移動し、次のコマンドで
@<code>{node} モジュールのインストールを行います。

//cmd{
$ cd ~\user\.vscode\extensions\<publisherId>.helloworld-<version>
$ npm install
//}

引数無しの @<code>{npm　install} コマンドは、 @<code>{package.json} の
@<code>{dependencies} に記述されたモジュールを全てインストールするそうです。 @<fn>{npm_install}

//quote{
Install the dependencies in the local node_modules folder.

(中略)

By default, npm install will install all modules listed as dependencies in package.json.
//}

「@<code>{exclude} から除外してしまえばいいのでは？」とも考えられるのですが、
その分 VSIX ファイルのサイズが大きくなってしまうので、あまりお勧めはできないところです。

これで VSCode を再起動し、改めて @<code>{HelloWorld} コマンドを実行してみてください。
メッセージが表示されればインストール完了です。

//footnote[npm_install][npm-install https://docs.npmjs.com/cli/install]

== マーケットプレイスへの公開

@<hd>{generate_vsix} にて VSIX ファイルが生成できていれば、
次のコマンドを実行するだけで即公開されます。
@<b>{特に公開の確認などはありません。}

//footnote[ontime_publish][もちろんrepository要素が記述されていなければYes/Noの選択肢が表示されますが、VSIXファイルを生成できていればそれ以外のERROR等で止まることはないと思います]

//cmd{
# package.json があるフォルダに移動
$ cd ~/helloworld
$ vsce publish
//}

//image[helloworld_published][HelloWorld拡張機能の公開（実際にやってみた）][scale=0.6]{
//}

Unpublish は次のコマンドで実行できます。

//cmd{
$ vsce unpublish
//}

=== 拡張機能のアップデート

公開中の拡張機能のアップデートを行う場合は、バージョンに応じたオプションを付与します。

//footnote[semver][Semantic Versioning 2.0.0 https://semver.org/]

//cmd{
$ vsce publish major|minor|patch|x.y.z
//}

 * major
 ** メジャーバージョンアップ … 1.0.0 → 2.0.0
 * minor
 ** マイナーバージョンアップ … 1.0.0 → 1.1.0
 * patch
 ** パッチ … 1.0.0 → 1.0.1
 * バージョン直接指定
 ** Semantic Versioning にしたがって直接指定も可能

オプション付きで @<code>{puclish} コマンドを実行すると、 @<code>{vsce} が
オプションに応じて自動的に @<code>{package.json} のバージョンをインクリメントしてくれるので、
手動でバージョンを調整する必要はありません。もちろん特定のバージョンを指定することも可能です。
至れり尽くせりです。

== まとめ

拡張機能の公開はとてもハードルが低く、サクッと作ってサクッと公開することができます。
Android などのモバイルアプリでは、審査があったり公開自体にある程度の費用が必要になったりしますが、
拡張機能は無料で公開し使ってもらうことが可能です。

最近は Vim や emacs などから VSCode に移住されている方も増えているようなので（執筆者観測範囲）、
ぜひみんなで使いやすい開発環境を作っていきましょう！やっていきです！