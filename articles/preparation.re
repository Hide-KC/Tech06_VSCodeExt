={preparation} 拡張機能開発の準備

この章では VSCode 拡張機能の開発環境を整えていきます。
また、「Hello World」という拡張機能を作り、メッセージの出力まで確認してみます。

開発に必要なツール等は次のとおりです。

 1. VSCode 本体
 2. Git（Git for Windows を使用）
 3. Node.js
 4. TypeScript コンパイラ
 5. Yeoman （拡張機能のひな形を生成するツール）

もし JavaScript を使って開発をする場合、TypeScript コンパイラは不要です。
本書では、筆者が型付き言語信者のため TypeScript を使用しますが、どうしても JavaScript で開発したい方は適宜読み替えをお願いします。

== VSCode のインストール

VSCode のインストーラは次の VSCode 公式サイト @<fn>{vscode_url} からダウンロードできます。

//footnote[vscode_url][Visual Studio Code https://code.visualstudio.com/]

//image[vscode_site][Visual Studio Code][scale=0.72]{
//}

ボタンを押すと自動的にダウンロードが始まります。
exe ファイルをダブルクリックし、画面の指示にしたがってインストールしましょう。
インストールが終わったら、念のためパスが通っているか確認します。

//cmd{
$ code -v
1.31.1
...
//}

バージョンが返ってくれば大丈夫です。筆者の環境では次のパスでした。

//list[path_code][VSCode パス]{
C:\Users\user\AppData\Local\Programs\Microsoft VS Code\bin
//}

== Git のインストール

今回は Git for Windows @<fn>{gfw_url} を使用します。

//footnote[gfw_url][Git for Windows https://gitforwindows.org/]

//image[gfw_site][Git for Windows][scale=0.72]{
//}

同様に画面の指示にしたがってインストールします。
こちらもインストールが終わったら、パスの確認をしておきましょう。

//cmd{
$ git --version
git version 2.20.1
//}

//list[path_git][Git for Windows パス]{
C:\Program Files\Git\cmd
//}

== Node.js のインストール

VSCode は Electron フレームワーク上（Node.js + Chromium）で構築されているため、
拡張機能の開発にも Node.js 環境 @<fn>{node_url} が必要になります。

//footnote[node_url][Node.jp https://nodejs.org/ja/]

//image[node_site][Node.js][scale=0.72]{
//}

同様に指示にしたがってインストールします。お約束のパス確認。

//cmd{
$ npm -v
6.4.1
$ node -v
v10.13.0
//}

//list[path_node][Node.js パス]{
C:\Program Files\nodejs
//}

== TypeScript コンパイラのインストール

次に TypeScript コンパイラをインストールします。
TypeScript で記述されたファイルはそのままでは実行できませんので、コンパイラを使用し JavaScript にコンパイルします。
前節で @<code>{npm} が使えるようになっていると思いますので、次のコマンドでインストールします。

//cmd{
$ npm install -g typescript
...
$ tsc -v
Version 3.3.3
//}

//list[path_tsc][Node モジュールのパス]{
C:\Users\user\AppData\Roaming\npm
//}

== Yeoman のインストール

VSCode 公式サイトでは、拡張機能の開発において簡単にひな形を生成できる Yeoman（ヨーマン）の Yo ツール@<fn>{yeoman_url}の使用を推奨しています。

//footnote[yeoman_url][Yeoman https://yeoman.io/]

//image[yeoman_site][Yeoman 公式サイト][scale=0.72]{
//}

Yo は次のコマンドでインストールします。
また、同時に VSCode 拡張機能のひな形を生成する generator-code モジュールもインストールします。@<fn>{generator_code}

//footnote[generator_code][Yo を使用するにあたって、テンプレートを記述した generator-* という拡張モジュールも必要となります。npm search yeoman-generator で検索もできます（searchは少し時間がかかるので注意）]

//cmd{
$ npm install -g yo generator-code
...
$ yo --version
2.0.5
//}

== Hello World!

これで準備が整いました。それではおなじみの Hello World! を出力してみましょう。@<fn>{base_site}

//footnote[base_site][Your First Extension に基づきます https://code.visualstudio.com/api/get-started/your-first-extension]

=== Yeoman で拡張機能のひな形を生成

拡張機能を開発するフォルダに移動し、次のコマンドを実行します。

//cmd{
$ yo code
//}

すると次のような@<del>{ひげおじさんの}アスキーアートと選択肢が表示されます。

//cmd{
$ yo code

     _-----_     ╭──────────────────────────╮
    |       |    │   Welcome to the Visual  │
    |--(o)--|    │   Studio Code Extension  │
   `---------´   │        generator!        │
    ( _´U`_ )    ╰──────────────────────────╯
    /___A___\   /
     |  ~  |
   __'.___.'__
 ´   `  |° ´ Y `

? What type of extension do you want to create? (Use arrow keys)
> New Extension (TypeScript)
  New Extension (JavaScript)
  New Color Theme
  New Language Support
  New Code Snippets
  New Keymap
  New Extension Pack
(Move up and down to reveal more choices)
//}

矢印を @<code>{New Extension (TypeScript)} に合わせて選択すると、作成する拡張機能の名前を聞かれます。
@<code>{HelloWorld} と入力してください。

//cmd{
# ? What's the name of your extension? HelloWorld
//}

次に拡張機能の識別子を聞かれるので、 @<code>{helloworld} と入力します。
そのまま Enter キーを叩いても大丈夫です。
なお、識別子は全て小文字かつアンダーバーは使用できないのでご注意ください。
ハイフンは使用可能です。@<fn>{ext_identifier}

//footnote[ext_identifier][マーケットプレイスへの公開時に、後ほど登場する publisher と組み合わせられて <publisher>.<extensionName> となります]

//cmd{
### Press <Enter> to choose default for all options below ###

# ? What's the identifier of your extension? helloworld
//}

次に拡張機能の詳細について聞かれます。空欄でも大丈夫です。

//cmd{
# ? What's the description of your extension? LEAVE BLANK
//}

次に TypeScript のチェックを厳格に行うかどうかを聞かれます。
 @<code>{Yes} とすると、生成される @<code>{tsconfig.json} ファイルの @<code>{strict} オプションが @<code>{true} になります。
後から必要に応じ緩めることもできるので、 @<code>{Yes} にしておきましょう。@<fn>{use_strict}

//footnote[use_strict][noImplicitAny, noImplicitThis, alwaysStrict など、さまざまなチェックオプションを有効にするとのこと https://www.typescriptlang.org/docs/handbook/compiler-options.html]

//cmd{
# ? Enable stricter TypeScript checking in 'tsconfig.json'? Yes
//}

次に @<code>{tslint.json} を作成するか聞かれます。TypeScript のリンターですね。
 @<code>{Yes} にしておきましょう。

//cmd{
# ? Setup linting using 'tslint'? Yes
//}

次に Git リポジトリを作成するか聞かれます。
ここも @<code>{Yes} で。

//cmd{
# ? Initialize a git repository? Yes
//}

最後に、パッケージマネージャを @<code>{npm} にするか @<code>{yarn} にするか聞かれます。
お好きな方で構わないと思いますが、とりあえず @<code>{npm} にしておきましょう。

//cmd{
# ? Which package manager to use? npm
//}

ズラズラとコードが走り、最終的にカレントディレクトリに @<code>{helloworld} というフォルダが生成されていると思います。
次のコマンドで VSCode に展開します。

//cmd{
$ code ./helloworld
//}

=== 拡張機能の実行

これから実行する拡張機能は、単純に次のような機能を有しています。

 * Hello World コマンドの実装
 * コマンドを実行すると、"Hello World!" というインフォメーションメッセージを表示する

フォルダの構成は @<list>{ext_tree} のようになっています。

//list[ext_tree][ファイル構成]{
HelloWorld
  ├ .vscode
  ├ node_modules
  ├ src
  │  └ extension.ts
  ├ .gitignore
  ├ .vscodeignore
  ├ package.json
  ├ …
//}

@<list>{ext_tree}のうち、この章で扱うのは次の 2 ファイルです。

 * @<code>{src} フォルダ
 ** ソースファイルを格納。 TypeScript（ @<code>{.ts} ）で記述する
 * @<code>{package.json}
 ** Node.js 使いにはおなじみであろうマニフェストファイル。
 モジュールの依存関係やエントリポイント、拡張機能の設定項目など、このファイルを基にコンパイルが行われる @<fn>{package_url}

//footnote[package_url][npm package.json 取扱説明書 http://liberty-technology.biz/PublicItems/npm/package.json.html]

@<code>{src/extension.ts} というファイルがあるので開いてみましょう。
ちなみに拡張機能のデフォルトのエントリポイントは、
この @<code>{extension.ts} を JavaScript ファイルにコンパイルした @<code>{./out/extension.js} です。

コメントを消すとこのような形になっています。

//listnum[def_extension][初期の extension.ts]{
import * as vscode from 'vscode';
export function activate(context: vscode.ExtensionContext) {
  console.log('Congratulations, your extension "helloworld" is now active!');

  let disposable = vscode.commands.registerCommand(
  'extension.helloWorld', () => {
    vscode.window.showInformationMessage('Hello World!');
  });

  context.subscriptions.push(disposable);
}

export function deactivate() {}
//}

なんとこのまますぐ実行できてしまうので、何も変えずにやってみましょう。
デバッグは @<code>{F5} で起動します。
すると、「Hello World 拡張機能」が有効になった状態で新しいウィンドウが立ち上がります。

では、新しいウィンドウで @<code>{Ctrl + Shift + P（または、表示 → コマンドパレット）} でコマンドパレットを開き、
「Hello World」 と打ち込んでみましょう。候補に @<code>{Hello World} が表示されると思うので、
そちらをクリックします（ @<img>{cmd_hello} ）。

//image[cmd_hello][Hello World コマンド][scale=1.0]{
//}

次のようなメッセージが画面右下に表示されると思います（ @<img>{message_hello} ）。

//image[message_hello][Hello World メッセージ][scale=1.0]{
//}

また、 @<code>{extension.ts} を開いているウィンドウのデバッグコンソール（ @<code>{Ctrl + Shift + Y} ）にも、
次のようなログが出力されていると思います。

//cmd{
Congratulations, your extension "helloworld" is now active!
//}

これで Yeoman で自動生成した拡張機能が動作することを確認できました。

次節で処理の流れを追ってみますが、その前に少しだけコードをいじってみましょう。
@<code>{showInformationMessage} を @<code>{showErrorMessage} に
変えることで、エラーメッセージが表示されるようになります。次に進む前に少しだけ試してみてください。

//image[message_error][エラーメッセージ][scale=1.0]{
//}

=== 中で行われていること

さて、実際の処理の流れはどうなっているのでしょうか。
デバッグ実行からメッセージ表示までは、簡単に次のような流れになっています。

//list[ext_flow][メッセージ表示までの流れ]{
（デバッグ実行）
　↓
コマンドの読み込み（コマンドパレットへの表示）
 - package.json contributes.commands
　↓
どんなイベントで拡張機能の activate メソッドを呼ぶか
 - package.json activationEvents
　↓
（Hello World コマンドのコール）
　↓
activate メソッドの実行
 - extension.ts activate メソッド
　↓
メッセージの表示
 - extension.ts showInformationMessage メソッド
//}

@<code>{package.json} 及び @<code>{extension.ts} の記述を確認します。

//listnum[def_package][package.json]{
{
  ...
  "activationEvents": [
    "onCommand:extension.helloWorld"
  ],
  "main": "./out/extension.js",
  "contributes": {
    "commands": [
      {
        "command": "extension.helloWorld",
        "title": "Hello World"
      }
    ]
  }
  ...
}
//}

//listnum[def_ext2][extension.ts（再掲）]{
import * as vscode from 'vscode';
export function activate(context: vscode.ExtensionContext) {
  console.log('Congratulations, your extension "helloworld" is now active!');

  let disposable = vscode.commands.registerCommand(
    'extension.helloWorld', () => {
      vscode.window.showInformationMessage('Hello World!');
    });

  context.subscriptions.push(disposable);
}

export function deactivate() {}
//}

==== コマンドの読み込み

@<code>{package.json} の @<code>{"contributes.commands"} で定義します。（@<list>{def_package}）

@<code>{"command"} はコマンドごとに一意に決められた ID 、
@<code>{"title"} はコマンドパレットに表示される際のコマンド名になります。

===={when_activate} どんなイベントで activate メソッドを呼ぶか

@<code>{package.json} の @<code>{"activationEvents"} で定義します。（@<list>{def_package}）

指定されたアクションが実行されたとき、
エントリポイント（ @<code>{./out/extension.js} ）の @<code>{activate} メソッドがコールされます。
Activation Events には、主に次のような種類があります。

//table[events_table][Activation Events]{
イベント	詳細
----------------------------------
onLanguage	指定の言語ファイルが開かれたとき
onCommand	指定のコマンドがコールされたとき
onDebug	デバッグセッションで拡張機能が起動されたとき
workspaceContains	指定のパターンにマッチするファイルが含まれていたとき
onFileSystem	特定のスキームでファイル等が開かれたとき（ftp,ssh等）
onView	指定のビューが展開されたとき
onUri	指定のUriで拡張機能が開かれたとき
onWebviewPanel	指定のViewTypeのWebViewを復元（展開）するとき
"*"	VSCodeが起動したとき
//}

さまざまなタイミングで @<code>{activate} メソッドをコールできるようになっているので、
目的に応じて設定してみてください。 @<fn>{webviewpanel} @<fn>{activation_events}

//footnote[webviewpanel][Webview API https://code.visualstudio.com/api/extension-guides/webview]
//footnote[activation_events][VSCode API Activation Events https://code.visualstudio.com/api/references/activation-events]

==== コマンド実行時の処理の読み込み

@<code>{registerCommand} で実装します。
第二引数には、 @<code>{Hello World} コマンドを実行した際の処理を実装します。

@<code>{registerCommand} はコマンド実行を待機するために @<code>{disposable} なオブジェクトを返しますが、
@<code>{activate} の最後に必ず @<code>{context.subscriptions} に @<code>{push} してください。
@<code>{context.subscriptions.push} をしておけば、ウィンドウを閉じる等で拡張機能が @<code>{deactivate} されたときに、
イベント待機用に確保していたメモリを自動的に解放してくれます。 @<fn>{events} @<fn>{subscriptions}

//footnote[events][VSCode API #Events https://code.visualstudio.com/api/references/vscode-api#events]
//footnote[subscriptions][VSCode API #ExtensionContext subscriptions 参照 https://code.visualstudio.com/api/references/vscode-api#ExtensionContext]

==== メッセージの表示

@<code>{registerCommand} のコールバックで実装された @<code>{vscode.window.showInformationMessage} メソッドで行います。
単純に @<code>{'Hello World!'} という文字列を渡しているだけですね。

== まとめ

拡張機能開発の準備から、もっとも簡単な拡張機能の実行までを駆け足で確認しました。
環境構築がメインだったので、実装も初期状態の確認程度でしたが、いかがでしたでしょうか。

開発するにあたってあるあるなミスとしては、次のようなものがあると思います。

 1. @<code>{"contributes.commands"} に追加し忘れる
 2. @<code>{"activationEvents"} に追加し忘れる
 3. コマンド ID を打ち間違える（Intellisense で自動補完したい）
 4. @<code>{registerCommand} を忘れる

ID 打ち間違えに関しては Node モジュールがありそうな気もしなくもないです。
コマンド主体の拡張機能については@<chap>{dev_command}、
テキストハイライトの拡張機能については@<chap>{dev_highlight}で解説しているので、そちらもご覧ください。

//raw[|latex|\pagebreak]

====[column] 拡張機能のデバッグ

言うまでもないかもしれませんが、拡張機能は Node.js パッケージです。

Node.js パッケージをデバッグする場合、普通なら @<code>{launch.json} のデバッガの種別（@<code>{type}）に @<code>{"node"} を
指定するところですが、拡張機能の @<code>{type} を見ると @<code>{extensionHost} となっています。

//list[launch_type][type - launch.json]{
{
  "version": "0.2.0",
  "configurations": [{
      "name": "Run Extension",
      "type": "extensionHost",
      ...
    }]
}
//}

公式によると、@<code>{Extension Host}は拡張機能のロードと実行を担当する Node.js のプロセスだそうです。

//quote{
Extension host is a Node.js process in VS Code responsible for loading and running extensions.

 - Extension Host https://code.visualstudio.com/api/advanced-topics/extension-host
//}

念のため GitHub ものぞいてみました。

//quote{
The extension host process runs code implemented by a plugin. To debug extensions (including those packaged with Code) which run in the extension host process, you can use VS Code itself.

 - How to Contribute https://github.com/Microsoft/vscode/wiki/How-to-Contribute
//}

拡張機能のデバッグ専用に作られているようですね。
ちなみに @<code>{node} にすると、同じようにデバッグが開始されますが、
リロードやステップインを行うコントロールバーが表示されなくなります（起動時一瞬だけ表示されます）。

詳細なドキュメントが見当たらなかったのでこれ以上の説明は難しいですが、
拡張機能をデバッグする場合は @<code>{node} ではなく @<code>{extensionHost} を指定しましょう。

====[/column]