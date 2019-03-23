={dev_command} コマンド系拡張機能の開発

この章では、ごく簡単なコマンドを搭載した拡張機能を開発してみます。
適宜外部の Node モジュールをインストールして開発を行っていきます。

== VSCode のコマンドについて

VSCode にはコマンドパレットという機能があり、VSCode のほぼすべての機能が
このコマンドパレットを通じて使用することができます。
たとえば、コマンドパレットに @<code>{New File} と打ち込むと、
新しいファイルを生成するためのコマンドが表示されます。

== ひな形の生成

拡張機能を作るにあたり、あらかじめひな形を生成しておきましょう。
@<chap>{preparation} を参考に生成してみてください。
拡張機能の名称は「GenerateFiles」とします。

//cmd{
$ yo code
...
# ? What's the name of your extension? GenerateFiles
# ? What's the identifier of your extension? generatefiles
//}

=={gf} ファイル・フォルダセットを生成する

コマンドひとつで、カレントフォルダに定型のファイル群を生成します。
ファイル群の構成は次のとおりです。 @<code>{root} 以下が生成されるファイルとなります。 @<fn>{workspace}

//footnote[workspace][ワークスペースは、 VSCode の「フォルダーを開く」で開いているフォルダを指します]

//list[gf_tree][ファイル構成]{
ワークスペース（カレントフォルダ）
  └ root（フォルダ名 YYYYMMDD-HHMMSS）
    ├ images
    └ content.md
//}

ファイルの生成に @<code>{fs} モジュール、現在時刻の取得に @<code>{moment} モジュールを使用します。
@<code>{fs} は Node.js の標準モジュール @<fn>{nodejs_modules} としてインストール済みのため、 @<code>{moment} をインストールしていきます。

//footnote[nodejs_modules][Node.js 標準モジュール https://nodejs.org/dist/latest-v10.x/docs/api/]

==== moment モジュールのインストール

ターミナルを開き（VSCode で @<code>{Ctrl+@} ）、次のコマンドで @<code>{moment} モジュールをインストールします。

//cmd{
$ cd node_modules
$ npm install moment
//}

インストール完了後、 @<code>{package.json} の @<code>{dependencies} が次のようになっていると思います。

//list[gf_dependencies][package.json - dependencies]{
{
  "dependencies": {
    "moment": "^2.24.0"
  }
}
//}

==== コマンドの登録

次に、コマンドパレットに表示する @<code>{extension.generatefiles} コマンドを config.yml の @<code>{contributes.commands} に定義します。

//list[gf_command][コマンドの定義 - package.json]{
{
  "contributes": {
    "commands": [
      {
        "command": "extension.generatefiles",
        "title": "Generate Files"
      }
    ]
  }
}
//}

==== Activation Events の登録

次に、拡張機能を起動させるための Activation Events を定義します。
@<chap>{preparation} で述べたとおり、Activation Events に定義されたアクションが発生すると、 @<code>{activate} メソッドが読み込まれ
拡張機能が起動します。

今回は Generate Files コマンドを叩くことで、拡張機能が起動するようにしてみます。

//list[gf_activationevents][Activation Events の定義 - package.json]{
{
  "activationEvents": [
    "onCommand:extension.generatefiles"
  ]
}
//}

==== activate の実装

次に、 @<code>{activate} メソッドを実装していきます。

//listnum[imp_gf][activate メソッドの実装 - extension.ts]{
import * as Moment from 'moment';

export function activate(context: vscode.ExtensionContext) {
  const disposable = vscode.commands.registerCommand(
    'extension.generatefiles', () => {
      //ワークスペースの取得（配列）
      const workspaces = vscode.workspace.workspaceFolders;

      if (workspaces !== undefined) {
        //現在日時の取得
        const now = Moment().format("YYYYMMDD-HHmmss");
        
        //カレントフォルダの取得
        const current = workspaces[0].uri.fsPath;

        //日時をフォルダ名にする
        const root = current + `\\${now}`;
        fs.mkdir(root, err => {
          if (err === null) {
            fs.mkdir(root + '\\images', err => {});
            fs.appendFile(root + '\\content.md', "", err => {});
          }
          
          //作成したルートフォルダを展開
          vscode.commands.executeCommand(
            'vscode.openFolder', vscode.Uri.parse(root), false);
        }
      }
    });

  //subscriptionsにpush
  context.subscriptions.push(disposable);
}
//}

VSCode ではマルチルートワークスペース @<fn>{multirootworkspace} という機能がサポートされており、
ひとつのウィンドウで複数のプロジェクトフォルダを展開することができます。
@<code>{vscode.workspace.workspaceFolders} では、現在展開しているすべてのフォルダを
配列として取得します。もしファイルを開いているだけの場合や、新規ウィンドウを開いただけの場合は
@<code>{undefined} が返ります。

今回は開いているフォルダはひとつとし、 @<code>{workspaces[0].uri.fsPath} でカレントフォルダパスを取得しました。 @<fn>{for_mac}

//listnum[workspaces][workspaceFolders]{
const workspaces = vscode.workspace.workspaceFolders;
...
const current = workspaces[0].uri.fsPath;
//}

//footnote[multirootworkspace][VSCode Multi-root Workspaces https://code.visualstudio.com/docs/editor/multi-root-workspaces]
//footnote[for_mac][Mac では workspaces［0］.uri.path とします。パスの区切りが"/"で返ります]

フォルダやファイルの生成には @<code>{fs} モジュールを使用しました。
#@# 余談ですが、 @<code>{fs.mkdir} や @<code>{fs.appendFile} は非同期メソッドでコールバック関数を引数にもつため、
#@# フォルダ生成後にコールバックでさらにファイルを生成するなども可能となります。

仕上げに、 @<code>{subscriptions} に @<code>{push} することで、
拡張機能が @<code>{deactivate} したとき（VSCode を閉じる等）に、確実にメモリを解放してやるようにします。

==== デバッグ実行

それでは実際に動作を確認してみます。
@<code>{F5} でデバッグを開始します。

初期状態ではフォルダが開いていないか、以前に開いたフォルダが展開されていると思いますので、
適当なフォルダを開いてみてください。

コマンドパレットを開き（ @<code>{Ctrl+Shift+P} ）、 @<code>{Generate Files} コマンドを実行します。
次のようなフォルダやファイルが生成されれば成功です（ @<img>{gf_result} ）。

//image[gf_result][生成されたファイル、フォルダ][scale=1.0]{
//}

=={indent} インデントを変換する

次はテキストエディタっぽく、インデントを半角４つから２つに変換する拡張機能を作ります。

 1. 現在アクティブになっているエディタ（画面）を取得する
 2. 全行をスキャンし、インデントを逐次 @<code>{replace} する

上記のような手順で変換を行います。

==== コマンドの登録

特段モジュールを @<code>{import} する必要がないため、コマンドの登録から始めます。
@<code>{4to2} コマンドを定義します。

//list[indent_command][コマンドの定義 - package.json]{
{
  "contributes": {
    "commands": [
      {
        "command": "extension.4to2",
        "title": "Indent 4 to 2"
      }
    ]
  }
}
//}

==== Activation Events の登録

コマンドを叩くことで拡張機能が起動するように、
Activation Events にコマンドを登録します。

//list[indent_activationevents][Activation Events の定義 - package.json]{
{
  "activationEvents": [
    "onCommand:extension.4to2"
  ]
}
//}

==== activate の実装

@<code>{activate} の実装です。
@<hd>{gf} と大きく違うところは、コマンドの実行をアクティブなエディタ単位にするため、
@<code>{registerTextEditorCommand} メソッドを使用しているところです。

//listnum[imp_indent][activate メソッドの実装 - extension.ts]{
export function activate(context: vscode.ExtensionContext) {
  const disposable = vscode.commands.registerTextEditorCommand(
    'extension.4to2', (textEditor, edit) => {

    //ドキュメントオブジェクトと行数を取得
    const document = textEditor.document;
    const lineCnt = document.lineCount;

    //全行を走査
    for (let lineNum = 0; lineNum < lineCnt; lineNum++){
      //１行を文字列で取得
      const line = document.lineAt(lineNum).text;
      edit.replace(
        //lineNum行目の行頭(0)から行末(line.length)を範囲指定
        new vscode.Range(lineNum, 0, lineNum, line.length),
        convert4To2(line)
      );
    }
  });

	context.subscriptions.push(disposable);
}

//正規表現で半角スペースを抽出し置換
function convert4To2(line: string): string {
  const _leadSpaces = /^\s*/.exec(line);
  
  if (_leadSpaces){
    const leadSpaces = _leadSpaces[0];
    const newLeadSpaces = leadSpaces.replace(/[ ]{4}|\t/g, '  ');
    return newLeadSpaces + line.replace(/^\s+/, '');
  }
  return line;
}
//}

==== デバッグ実行

@<code>{F5} でデバッグを開始します。
半角スペース4つでインデントされたテキストファイルを開いてください。
コマンドパレットを開き（ @<code>{Ctrl+Shift+P} ）、 @<code>{Indent 4 to 2} コマンドを実行します。

//image[4to2][インデント変換前後][scale=1.0]{
//}

== まとめ

VSCode API や各種 Node モジュールを使用することで、さまざまな機能を持ったコマンドを実装することができます。
処理の記述自体はなにがしかの開発経験があれば、比較的簡単に書けるのではないでしょうか。

他にも @<code>{request} モジュールを使って @<code>{GET}, @<code>{POST} や、
OAuth 認証なども通すことができます（ @<chap>{hatena_oauth} 参照）。 @<fn>{mohaya}

よりエディタっぽい拡張機能については、 @<chap>{dev_highlight} も参考にしてみてください。

//footnote[mohaya][もはやエディタとして使っていない件]

#@# //embed[latex]{
#@# \clearpage
#@# //}

//raw[|latex|\pagebreak]

====[column] コマンドを複数実装する場合

実装するコマンドが複数になった場合、 @<code>{Disposable} オブジェクトひとつひとつを
@<code>{subscriptions} に追加するのは少し面倒です。

そこで @<code>{Disposable} の配列を作り、
@<code>{subscriptions.concat} でまとめて追加（結合）してみます。

//list[concat][concat]{
//Disposableの配列
const disposables: vscode.Disposable[] = [];

//テンプレートフォルダの生成
const generateFiles = () => {
  //フォルダの生成実装
  ...
};

//配列にpush
disposables.push(
  vscode.commands.registerCommand(
    'extension.generatefiles', generateFiles
  )
);

//他にもコマンドを実装
...
disposables.push(...)

//最後にsubscriptionsにconcat
context.subscriptions.concat(disposables);
//}

必要に応じて使い分けてみてください。

====[/column]