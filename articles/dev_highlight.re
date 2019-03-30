#@# https://code.visualstudio.com/api/language-extensions/syntax-highlight-guide
#@# シバン http://mkawabe.hatenablog.com/entry/2016/05/20/171729
#@# Syntax Definitions http://docs.sublimetext.info/en/latest/reference/syntaxdefs.html
#@# Sublime Text Scope Naming https://www.sublimetext.com/docs/3/scope_naming.html
#@# Nyaruko lang https://github.com/masarakki/nyaruko_lang

={dev_highlight} シンタックスハイライトの開発

この章では、ソースコードを記述するための Abc 言語という架空の言語を考え、
シンタックスハイライト（長いので以下、ハイライト）を開発してみます。

@<chapref>{dev_command} とは異なり、すべて JSON での記述となります。
また、基本的に VSCode のチュートリアルを参考に進めていきます。 @<fn>{syntax_highlight}

//footnote[syntax_highlight][VSCode Syntax Highlight Guide https://code.visualstudio.com/api/language-extensions/syntax-highlight-guide]

== ハイライトはどのように行われるか

開発の前段階として、そもそもハイライトはどのように行われるのかを考えます。
@<list>{sample_js}を記述した JavaScript ファイル（ @<code>{sample.js} ）を用意してください。
このコードを例に説明を行います。

//list[sample_js][変数宣言 - JavaScript]{
var foo = 1;
//}

==={token} トークンへの分解

ハイライトを行う場合、コードをひとつひとつの語句に区切った@<b>{トークン}に分解することが必要です。
今回は@<list>{sample_js}を次のようなトークンに分解しましょう。

//list[to_tokens][トークンへの分解]{
  var     foo     =      1      ;
  ^^^     ^^^     ^      ^      ^
変数宣言  変数名  演算子  定数  行末文字
//}

もちろんこれらの語句の間のスペースもトークンとして扱えますが、今回は無視します。 @<fn>{space_hl}
さてこれらの各トークンに色を付けるわけですが、VSCode が色を付けるために
@<b>{スコープ}という目印のようなものを割り当てていきます。

//footnote[space_hl][行末のスペースをハイライトする拡張機能とかもありますね]

==={scope} スコープの割り当て

スコープは、VSCode がトークンに色を付けるときに目印とする属性のようなものです。
@<code>{sample.js}にはすでに JavaScript ファイルとしてスコープが割り当てられているので、
@<b>{Inspect TM Scope} コマンドによりスコープを確認してみましょう。

@<code>{Ctrl+Shift+P} でコマンドパレットを開き、 @<code>{Inspect TM Scope} と打ち込みます。
コマンドが見つかったら選択し、 @<code>{var} のところにカーソルを合わせると次のようなホバーウィンドウが表示されます。

//image[inspect_tm_scope][Inspect TM Scope][scale=0.72]{
//}

上から順に次のような情報が記載されています。

 1. 検査中のトークン
 2. トークンのメタデータ
 3. トークンに適用されているカラーテーマのルール
 4. 割り当てられたスコープのリスト。上に行くほどより具体的なスコープになる

色付けに使われるスコープは、スコープリストの一番上のものになります。 @<fn>{meta_scope}

//footnote[meta_scope][@<code>{meta.*}スコープは基本的にスタイルの適用には使われず、ブロックとしてくくるためのスコープ。たとえば@<code>{meta.function}はfunctionブロック全体を指すスコープ、@<code>{meta.var}は変数宣言ステートメント全体を指すスコープ、など]

//table[scope_table][トークンと割り当てられたスコープ]{
トークン	スコープ	適用されるカラーテーマのルール
----------
var	@<code>{storage.type.js}	@<code>{storage.type}
foo	@<code>{variable.other.readwrite.js}	@<code>{variable}
=	@<code>{keyword.operator.assignment.js}	@<code>{keyword.operator}
1	@<code>{constant.numeric.decimal.js}	@<code>{constant.numeric}
;	@<code>{punctuation.terminator.statement.js}	@<code>{No_theme_selector}
//}

スコープが指定されていれば、VSCode は現在有効になっているカラーテーマから適用すべき色を引っ張ってきます。

==={color_theme} カラーテーマから色を適用する

筆者は現在、カラーテーマとして @<code>{Dark+（dark_plus.json）} を使用しています。
この中に @<code>{storage.type} や @<code>{variable} スコープをキーとして、色の定義がされていますので確認してみましょう。
デフォルトのカラーテーマは次のパスに存在します。 @<fn>{hukai}
@<br>{}

//footnote[hukai][やたら深い]

@<code>{C:\Users\user\AppData\Local\Programs\Microsoft VS Code\resources\app\extensions\theme-defaults\themes}
@<br>{}

@<code>{dark_plus.json} は外部ファイルを読み込んでおり、
この中から@<table>{scope_table}に関するスコープを探してみます。

//list[dark_dependency][外部ファイルとの依存関係]{
dark_defaults.json
  └ dark_vs.json
    └ dark_plus.json
//}

次のような要素が見つかったでしょうか。

//list[dark_string][スコープと色の対応]{
// dark_plus.json
{
  {
    "name": "Variable and parameter name",
    "scope": [
      "variable",
      "meta.definition.variable.name",
      "support.variable",
      "entity.name.variable"
    ],
    "settings": {"foreground": "#9CDCFE"}
  }
}

// dark_vs.json
{
  {
    "scope": "storage.type",
    "settings": {"foreground": "#569cd6"}
  },
  {
    "scope": "keyword.operator",
    "settings": {"foreground": "#d4d4d4"}
  },
  {
    "scope": ["constant.numeric"],
    "settings": {"foreground": "#b5cea8"}
  }
}
//}

ここで指定された色が、@<img>{inspect_tm_scope} の @<code>{foreground} として色付けに
使われていることが分かると思います。

このように、適切にトークンに分解し、スコープを割り当てることで、ハイライトを行うことができます。
スコープは TextMate 文法を使用しており、デフォルトのカラーテーマのスコープはこの規則にしたがっています。
そのため、原則この文法にしたがって記述していきましょう。 @<fn>{textmate_language_grammars} @<fn>{sublime_scope_naming}

//footnote[textmate_language_grammars][TextMate Language Grammars https://macromates.com/manual/ja/language_grammars]
//footnote[sublime_scope_naming][Sublime Text Scope Naming https://www.sublimetext.com/docs/3/scope_naming.html]

== ひな形の生成

以降のページでは、実際に開発に関するファイルを見ながら解説していこうと思います。
Yeoman でひな形を作りましょう。

//cmd{
$ yo code

? What type of extension do you want to create? (Use arrow keys)
  New Extension (TypeScript)
  New Extension (JavaScript)
  New Color Theme
> New Language Support
  New Code Snippets
  New Keymap
  New Extension Pack
(Move up and down to reveal more choices)
//}

@<code>{New　Language　Support} を選択することで、
ハイライトや入力補完などの言語用拡張機能のひな形を生成します。
@<br>{}

最初に、インポートするファイルや URL があるかどうか聞かれます。
特にインポートするものもないので、空欄で OK です。

//cmd{
? URL or file to import, or none for new:
//}

次に拡張機能の名前を聞かれます。
Abc 言語という架空の言語を対象にするため、 Abc Lang とでもしておきます。

//cmd{
? What's the name of your extension? Abc Lang
//}

次に拡張機能の識別子を聞かれます。
拡張機能の名前から @<code>{abc-lang} と予測してくれるので、このままでいいでしょう。
Enter キーを押すだけで大丈夫です。

//cmd{
? What's the identifier of your extension? abc-lang
//}

次に拡張機能の詳細を聞かれます。
空欄でも大丈夫です。

//cmd{
? What's the description of your extension? Syntax highlighting for abc lang
//}

次にサポートする言語の id を聞かれます。
Abc 言語なので @<code>{abc} としておきます。@<fn>{known_language_ids}

//footnote[known_language_ids][Known language identifiers https://code.visualstudio.com/docs/languages/identifiers]

//cmd{
Enter the id of the language. The id is an identifier and is single,
lower-case name such as 'php', 'javascript'
? Language id: abc
//}

次にサポートする言語の名称を聞かれます。
ここで入力された名称が VSCode の右下（Language Mode）に表示されるようになります。
Abc 言語なので @<code>{Abc} としておきます。

//cmd{
Enter the name of the language. The name will be shown in the VS Code
editor mode selector.
? Language name: Abc
//}

次にサポートする拡張子を聞かれます。
Abc 言語なので @<code>{.abc} としておきます。
なお、拡張子の区別にあたっては Case sensitive なようで、
後ほど @<code>{package.json} にてエイリアスを設定します。

//cmd{
Enter the file extensions of the language. Use commas to separate
multiple entries (e.g. .ruby, .rb)
? File extensions: .abc
//}

最後にルートスコープ名を聞かれます。 @<fn>{textmate_scopename}
ルートスコープとは、@<b>{テキスト全体を指すスコープ}です。

TextMate のドキュメントによると、記述するものがソースコードであれば @<code>{source.<言語名>}を使用し、
文書であれば @<code>{text.<言語名>} とするとのことです。
今回はソースコードを記述するための言語なので、 @<code>{source.abc} としました。

//footnote[textmate_scopename][TextMate Language Grammar https://macromates.com/manual/en/language_grammars]

//cmd{
Enter the root scope name of the grammar (e.g. source.ruby)
? Scope names: source.abc
//}

これでカレントディレクトリに @<code>{abc-lang} というフォルダが生成されたと思います。
次のコマンドで VSCode に展開しましょう。

//cmd{
$ code ./abc-lang
//}

== ファイル構成

主なファイル構成は次のようになっています。

//list[abc_tree][ファイル構成]{
ワークスペース（カレントフォルダ）
  ├ .vscode
  │  └ launch.json
  ├ syntaxes
  │  └ abc.tmLanguage.json
  ├ language-configuration.json
  ├ package.json
  ├ ...
//}

 * package.json
 ** おなじみのマニフェストファイル。定義した言語の構文定義、入力規則の紐づけをする
 * abc.tmLanguage.json @<fn>{tm}
 ** @<b>{構文定義ファイル} : if, for やコメント等のハイライトを定義する
 * language-configuration.json
 ** @<b>{言語の入力規則ファイル} : カッコの自動クローズやインデントの自動増減等、自動補完関係を定義する

 次節から、ファイルの記述を確認していきます。

//footnote[tm][tm は TextMate の意、だそうです]

#@# =={eg_highlight} ハイライトの例

#@# ハイライトがついていない（定義されていない）場合、
#@# 文字列は白色（ライトテーマの場合は黒）で表されます。

#@# ごく簡単なハイライトであれば、 @<code>{tmLanguage.json} を触るだけで大丈夫なので、
#@# 少し実践してみましょう。@<list>{skeleton_tmlang} は @<code>{tmLanguage.json} の基本的な要素になります。

#@# //list[skeleton_tmlang][tmLanguage.json の基本的な要素]{
#@# {
#@#   "scopeName": "source.abc",
#@#   "$schema": "https://~~~",
#@#   "name": "Abc",
#@#   "patterns": [
#@#     ...
#@#   ],
#@#   "repository": {
#@#     ...
#@#   }
#@# }
#@# //}

#@# たとえば、ダブルクォートに囲まれた部分を、ダブルクォートを含めて "文字列" としてハイライトしてみます。
#@# ハイライトの定義は次のようになります。

#@# //list[doublequote_tmlang][文字列ハイライト]{
#@# {
#@#   "patterns": [{ "include": "#strings" }]
#@#   "repository": {
#@#     "strings": {
#@#       "patterns": [{
#@#         "begin": "\"",
#@#         "end": "\"",
#@#         "name": "string.quoted.double.abc"
#@#       }]
#@#     }
#@#   }
#@# }
#@# //}

#@# @<code>{repository} 下に @<code>{strings} を定義し、さらにその下に @<code>{patterns} を定義します。
#@# 今回は、「"ダブルクォートで囲まれた文字列"」をハイライトするにあたり、 @<code>{begin,end} と
#@# @<code>{string.quoted.double} スコープとを定義しました。

#@# これは、正規表現で指定された文字列（ @<code>{"\""} ）で開始され、かつ終わる（ @<code>{"\""} ）
#@# コードの部分に、 @<code>{string.quoted.double.abc} スコープを割り当てることを意味します。

#@# //list[eg_hoge][スコープの割当て]{
#@# var foo = "bar";
#@#           ^^^^^
#@#   string.quoted.double.abc scope
#@# //}

#@# @<code>{string.quoted.double} スコープが割り当てられたため、コードにはカラーテーマで紐づけられた
#@# @<code>{string} の色が適用されます。@<code>{string} は @<code>{dark_vs.json} に記述されているので探してみてください。

#@# この @<code>{strings} を ルートスコープ @<code>{source.abc} 直下の @<code>{patterns}（以降、ルートパターンと呼びます。） に
#@# @<code>{include} することで、ハイライトが有効になります。
#@# @<br>{}

#@# 次節からはもう少し具体的な @<code>{tmLanguage.json} の定義について記述していきますが、
#@# その前にまだ生成されたファイルの中身を確認していなかったので、順番に見てきましょう。

== package.json

まずは @<code>{package.json} を見てみます。

//list[package_abc][package.json]{
{
  "name": "abc-lang", // ← 拡張機能の識別子
  "displayName": "Abc Lang", // ← 拡張機能名
  "description": "Syntax highlighting for abc lang",
  "version": "0.0.1",
  "engines": {
    "vscode": "^1.31.0"
  },
  "categories": [
    "Programming Languages" // ← 拡張機能のカテゴリ
  ],
  "contributes": {
    // 入力規則
    "languages": [
      {
        "id": "abc",
        "aliases": ["Abc", "abc"],
        "extensions": [".abc"],
        "configuration": "./language-configuration.json"
      }
    ],
    // 構文定義
    "grammars": [
      {
        "language": "abc",
        "scopeName": "source.abc",
        "path": "./syntaxes/abc.tmLanguage.json"
      }
    ]
  }
}
//}

@<code>{cotributes} 要素にて、Abc 言語と入力規則（ @<code>{language-configuration.json} ）及び
構文定義（ @<code>{abc.tmLanguage.json} ）との紐づけをしています。

===={languages} languages 要素

@<code>{languages} 要素で、入力規則を適用するファイルの種類を指定します。
子要素として @<table>{languages_els} を指定できます。

//table[languages_els][languages 要素]{
要素名	解説
----------
id	VSCode API で使用する ID。getLanguageId() や onLanguage など
aliases	id のエイリアス。配列で指定。ひとつめは分かりやすい名前にしておく @<fn>{contributes_languages}
extensions	適用する拡張子。配列で指定
configuration	入力規則ファイルとの紐づけ
firstLine	ファイルの一行目を読んで判別する（シバン、シェバン）
filenamePatterns	マッチするファイル名のパターン。配列で指定
filenames	一致するファイル名。配列で指定
mimetypes	MIME 型 @<fn>{mime_types} の判別。配列で指定
//}

基本的に、 VSCode でファイルを開いたときに判別が行われます。

//footnote[contributes_languages][contributes.languages https://code.visualstudio.com/api/references/contribution-points#contributes.languages]
//footnote[mime_types][MIME型について https://developer.mozilla.org/ja/docs/Web/HTTP/Basics_of_HTTP/MIME_types]

===={grammars} grammars 要素

@<code>{grammars} 要素で、どの言語にどのようなハイライト処理を行うかを指定します。

//table[grammars_els][grammars 要素]{
要素名	解説
----------
language	構文定義を適用する言語 ID
scopeName	スコープ名。@<code>{source.abc}
path	適用する構文定義ファイルのパス
injectTo	inject する構文定義の指定
embeddedLanguages	親言語に埋め込む言語の指定
tokenTypes	トークンタイプの指定
//}

言語 ID は @<hd>{languages} で定義したような @<code>{abc} や、 @<code>{java,python} といった既定の言語の ID となります。 @<fn>{known_lang_ids}

//footnote[known_lang_ids][Known language identifiers https://code.visualstudio.com/docs/languages/identifiers]

=={lang_config} language-configuration.json

入力規則を記述した @<code>{language-configuration.json} を見てみます。

//list[lang_config][language-configuration.json]{
{
  "comments": {
    // symbol used for single line comment.
    // Remove this entry if your language does not support line comments
    "lineComment": "//",
    // symbols used for start and end a block comment.
    // Remove this entry if your language does not support block comments
    "blockComment": [ "/*", "*/" ]
  },
  // symbols used as brackets
  "brackets": [
    ["{", "}"],
    ["[", "]"],
    ["(", ")"]
  ],
  // symbols that are auto closed when typing
  "autoClosingPairs": [
    ["{", "}"],
    ["[", "]"],
    ["(", ")"],
    ["\"", "\""],
    ["'", "'"]
  ],
  // symbols that that can be used to surround a selection
  "surroundingPairs": [
    ["{", "}"],
    ["[", "]"],
    ["(", ")"],
    ["\"", "\""],
    ["'", "'"]
  ]
}
//}

定義可能な要素には次のようなものがあります。

//table[lang_config_els][language-configuration 要素]{
要素名	解説
----------
comments	コメントアウト用の文字の定義
brackets	使用するブラケット記号の定義
autoClosingPairs	対として自動補完する記号の定義
surroundingPairs	ドラッグした領域を囲む記号の定義
autoCloseBefore	autoClosingPairs と組み合わせて使用
folding	言語の折り畳み定義
wordPattern	プログラミング言語で単語と見なされるものを定義
indentationRules	エディタが現在の行または次の行のインデントを調整する方法を定義
//}

@<code>{autoCloseBefore} は少し特殊で、 @<code>{autoClosingPairs} と組み合わせて使用します。
デフォルトでは、VSCode はカーソルの直後に空白がある場合にのみペアを閉じます。
したがって、次のコードを入力しても自動クローズは行われません。

//list[autoclose_sample][autoCloseBefore の動作]{
const Component = () =>
  <div className={>
                  ^ Does not get autoclosed by default
  </div>
//}

この動作を上書きし、自動クローズが働くようにします。

//list[autoclose_define][autoCloseBefore の定義（例）]{
"autoCloseBefore": ";:.,=}])> \n\t"
//}

@<table>{lang_config_els} に挙げた要素の詳細な挙動については、公式ドキュメントをご覧ください。 @<fn>{docs_lang_config}

//footnote[docs_lang_config][Language Configuration Guide https://code.visualstudio.com/api/language-extensions/language-configuration-guide]

=={tmLanguage} abc.tmLanguage.json

次に @<code>{abc.tmLanguage.json} を見てみます。

@<code>{repository} にハイライトする部分のパターンを作り、
ルートスコープ（ @<code>{source.abc} ）の直下の @<code>{patterns}（以降、ルートパターンと呼びます。）に @<code>{include} することで
ハイライトが有効になります。
多少分かりづらいかもしれませんので、@<hd>{impl_highlight}もあわせてご覧いただければ幸いです。

パターンは基本的に正規表現で記述し、マッチしたトークンに、指定したスコープが付与されます。

//list[tmlang_list][abc.tmLanguage.json]{
{
  "scopeName": "source.abc",
  "$schema": "https://~~~",
  "name": "Abc",
  "patterns": [
    { "include": "#keywords" },
    { "include": "#strings" }
  ],
  "repository": {
    "keywords": {
      "patterns": [
        {
          "name": "keyword.control.abc",
          "match": "\\b(if|while|for|return)\\b"
        }
      ]
    },
    "strings": {
      "name": "string.quoted.double.abc",
      "begin": "\"",
      "end": "\"",
      "patterns": [
        {
          "name": "constant.character.escape.abc",
          "match": "\\\\."
        }
      ]
    }
  }
}
//}

@<list>{tmlang_list} は Yeoman で生成した初期の状態ですが、すでに２つのパターンが定義されています。

 1. "keywords" - 制御構文（ @<code>{if,while,for,return} ）
 2. "strings" - 文字列と文字列中のエスケープ文字（ @<code>{\\.} ）

@<code>{if,while,for,return} を Abc 言語の制御構文として定義し、既定の @<code>{keyword.control} に応じた
ハイライトを行います。

次に、エスケープを定義した @<code>{strings} ですが、 @<code>{keywords} とは異なり @<code>{patterns} がネストされています。
このようなとき、次の流れでマッチを行っています。

//list[strings_flow][エスケープ文字のマッチ]{
ダブルクォートで囲まれた部分を検知する
　↓
マッチした範囲に @<code>{string.quoted.double} スコープを割り当て、
さらに @<code>{patterns} にマッチする文字列を検知する
　↓
正規表現で「バックスラッシュ＋一文字」があれば、エスケープ文字として検知し
@<code>{constant.character.escape} スコープを割り当てる
//}

検知文字列は正規表現で記述されるので、ダブルクォート及びバックスラッシュはエスケープしています。
JSON におけるエスケープの記述には少しひねりがあり、バックスラッシュは4つ必要となります。 @<fn>{escape_char}

//footnote[escape_char][改行コードなら「\n」を「\」でエスケープしないといけないため、正規表現では「\\\\.」となる https://tools.ietf.org/html/rfc8259]

== デバッグで確認してみる

さて、主要な３ファイルの中身を確認してきたので、一度 @<code>{F5} でデバッグを開始してみます。
拡張子を @<code>{.abc} としてファイルを生成し、適当に打ち込んでみてください。
入力規則に応じて、カッコの自動クローズやインデントが働くと思います。
ここでは次のようにしてみました。

//list[sample_abc][初期状態のハイライト]{
// 白の場合はハイライト無し
a( ← 丸カッコ 黄色
  b( ← 丸カッコ マゼンタ
    c() { ←　丸カッコ シアン, 波カッコ シアン

      // 一行コメント             ← 白

      /* コメントブロック \n  */  ← 白

      //TODO トゥードゥー！       ← 白

      "文字列\n"                 ← ハイライト付き

      '文字列\n'                 ← すべて白

      var foo: int = 0;          ← すべて白
    }
  )
)
//}

筆者は現在 Dark+ (@<code>{dark_plus.json}) を VSCode のテーマとして設定しているので、
上記のような配色となっていました。
@<list>{sample_abc} において白とされている部分が、現在ハイライトが定義されていない部分になります。

それでは、次節から @<list>{sample_abc} を使ってハイライトの定義を行っていきます。

=={impl_highlight} 各種ハイライトの定義

=== エスケープ文字をハイライトする

コメントのハイライトから手を付けたいところですが、
その前に @<list>{tmlang_list} のエスケープのハイライトを分離しておきます。
というのも、エスケープはコメント、ダブルクォートで囲まれた文字列、
シングルクォートで囲まれた文字列など、さまざまなところで使う可能性があるためです。
さらに、定数を数値と文字列とで分けるなど、できるだけ細かい単位で構文を作っておくと、
@<code>{include} を使って構文の再利用ができるので便利です。

@<code>{tmLanguage.json} の @<code>{repository} に次の要素を追加します。
ちょうど @<code>{strings} のネストの中身を抜き取った形ですね。

//list[escape-character][escape-character 要素]{
{
  "repository": {
    "escape-character": {
      "patterns": [
        {
          "name": "constant.character.escape.abc",
          "match": "\\\\.",
        }
      ]
    }
  }
}
//}

対応するカラーテーマのスコープは次のとおりです。

//list[theme_escape][対応するスコープ - dark_plus.json]{
{
  "scope": "constant.character.escape",
  "settings": {"foreground": "#d7ba7d"}
}
//}

==={impl_comments} コメントをハイライトする

コメントには、一行コメントとコメントブロックが使えるものとします。
@<code>{comment.*} スコープを割り当てることで、現在適用しているカラーテーマの
@<code>{comment} のハイライトが適用されます。

また、 @<code>{patterns} をネストすることで、エスケープ文字には
別のハイライトが適用されるようになります。

//list[comment_tmlang][コメントの定義]{
{
  "repository": {
    "comments-block": {
      "name": "comment.block.abc",
      "begin": "/\\*",
      "end": "\\*/",
      "patterns": [
        {"include": "#escape-character"}
      ]
    },
    "comments-line": {
      "name": "comment.line.double-slash.abc",
      "begin": "//",
      "end": "\n",
      "patterns": [
        {
          "include": "#escape-character"
        }
      ]
    }
  }
}
//}

これで、たとえば@<list>{eg_comment}のような行があった場合、スコープが割り振られます。

//list[eg_comment][コメント行]{
  /* コメント */
  ^^^^^^^^^^^^^
  comment.block.abc scope

  var foo = "bar"; // コメント
                   ^^^^^^^^^^
          comment.line.double-slash.abc scope

//}

対応するカラーテーマのスコープは次のとおりです。

//list[theme_comment][対応するスコープ - dark_vs.json]{
{
  "scope": "comment",
  "settings": {
    "foreground": "#6A9955"
  }
}
//}

=== 文字列をハイライトする

シングルクォート及びダブルクォートで囲まれた部分に
@<code>{string.quoted} スコープを割り当て、文字列としてハイライトします。

//list[string_pattern_tmlang][文字列の定義]{
{
  "repository": {
    "strings-double": {
      "name": "string.quoted.double.abc",
      "begin": "\"",
      "end": "\"",
      "patterns": [
        {
          "include": "#escape-character"
        }
      ]
    },
    "strings-single": {
      "name": "string.quoted.single.abc",
      "begin": "'",
      "end": "'",
      "patterns": [
        {
          "include": "#escape-character"
        }
      ]
    }
  }
}
//}

これらを直接ルートパターンに @<code>{include} してもいいですが、今後似たようなパターンが増えてきたときに
大量の @<code>{include} を記述するようになってしまいます。
ここは文字列としてひとまとめに扱いたいので、次のように @<code>{strings} を定義して囲ってしまいます。

//list[strings_tmlang][文字列の定義を纏める]{
{
  "repository": {
    "strings": {
      "patterns": [
        { "include": "#strings-double" },
        { "include": "#strings-single" }
      ],
      "repository": {
        "strings-double": {
          ...
        },
        "strings-single": {
          ...
        }
      }
    }
  }
}
//}

この @<code>{strings} をルートパターンに @<code>{include} することで、文字列のハイライトが有効になります。
対応するカラーテーマのスコープは次のとおりです。

//list[theme_string][対応するスコープ - dark_plus.json]{
{
  "scope": "string",
  "settings": {
    "foreground": "#ce9178"
  }
}
//}

=== 変数宣言をハイライトする

次の変数宣言を考え、ハイライトしてみます。 @<fn>{qiita_make_highlight}

//footnote[qiita_make_highlight][超参考にしました。Visual Studio CodeやAtomのシンタックスハイライト拡張機能を作る https://qiita.com/maxfie1d/items/51af2984b7a628c41a94]

//list[var_first_tmlang][ハイライトする変数宣言]{
var foo: int = 0
//}

この宣言をトークンに分解すると、次の要素が含まれています。このあと割り当てるスコープも一緒に記載しました。

//table[explode_tokens][トークンと割り当てるスコープ]{
トークン	スコープ	詳細
----------
var,val	@<code>{storage.modifier}	変数宣言
foo	@<code>{variable.name}	変数名
int,float,string	@<code>{storage.type}	変数の型
=	@<code>{keyword.operator.assignment}	代入演算子
0	@<code>{constant.numeric.integer}	定数値
//}

==== スコープの割り当て

それでは実装してみます。 @<code>{abc.tmLanguage.json} の @<code>{repository} 以下に @<code>{var_declaration} を定義し、
次のようにマッチングとスコープを記述しました。

//list[attach_scope][スコープの割り当て]{
{
  "var_declaration": {
    "patterns": [
      {"include": "#var"},
      {"include": "#var_name"},
      {"include": "#var_type"},
      {"include": "#assignment"},
      {"include": "#constants"}
    ],
    "repository": {
      // 変数宣言
      "var": {
        "patterns": [
          {"match": "\\b(var|val)\\b", "name": "storage.type.abc"}
        ]
      },
      // 変数名
      "var_name": {
        "patterns": [
          {
            "match": "\\b([^0-9][a-zA-Z0-9]+)\\s*:",
            "captures": {
              "1": {"name": "variable.name.abc"}
            }
          }
        ]
      },
      // 変数の型
      "var_type": {
        "patterns": [
          {"match": "\\b(int|float|string)\\b", "name": "storage.type.abc"}
        ]
      },
      // 演算子
      "assignment": {
        "patterns": [
          {"match": "=", "name": "keyword.operator.assignment.abc"}
        ]
      },
      // 値
      "constants": {
        "patterns": [
          {"include": "#constant_numeric"},
          {"include": "#strings"}
        ],
        "repository": {
          "constant_numeric": {
            "patterns": [
              {"match": "-?[0-9]+", "name": "constant.numeric.integer.abc"}
            ]
          }
        }
      }
    }
  }
}
//}

@<code>{var_name} では、 @<code>{captures} という要素を使用しています。
これは、1行前の @<code>{match} にてグループ化した部分にインデックスでスコープを
設定している形となります。
巷の正規表現で、グループを取得するときと同じですね。

仕上げに、定義した @<code>{var_declaration} をルートパターンに追加することで完成です。
このような感じで（といってもモノクロですが）、きれいにハイライトされると思います。

//image[var_highlight][変数宣言のハイライト][scale=1.0]{
//}

#@# ==== 分解する

#@# まず最初に、トークンへの分解を行います。
#@# 変数宣言は正規表現で次のように表されます。

#@# //list[regex_explosion][分解用の正規表現]{
#@# (\\S+)\\s+(\\S+)\\s+(\\S+)\\s+(\\S+)\\s+(=)\\s+(\\S+)

#@# \\S … 半角スペース、タブ、改行以外の1文字
#@# \\s … 半角スペース、タブ、改行のどれか1文字
#@# + … 直前の文字の1文字以上の繰り返し
#@# //}

#@# 巷の正規表現と同じように丸カッコでグルーピングしていますが、
#@# 次のように @<code>{captures} によりグループごとにスコープを割り当てることができます。
#@# といってもまだ @<code>{include} 以下は定義していませんが……。

#@# //list[explosion][要素への分解]{
#@# {
#@#   "repository": {
#@#     "var_declaration":{
#@#       "patterns": [
#@#         {
#@#           "match": "(\\S+)\\s+(\\S+)\\s+(\\S+)\\s+(\\S+)\\s+(=)\\s+(\\S+)",
#@#           "captures": {
#@#             "1": { "patterns": [{"include": "#var_access"}] },
#@#             "2": { "patterns": [{"include": "#var_static"}] },
#@#             "3": { "patterns": [{"include": "#var_type"}] },
#@#             "4": { "patterns": [{"include": "#var_name"}] },
#@#             "5": { "patterns": [{"include": "#var_operator"}] },
#@#             "6": { "patterns": [{"include": "#constants"}] }
#@#           }
#@#         }
#@#       ],
#@#       "repository": {
#@#         // ここにvar_xxxを定義していく
#@#         ...
#@#       }
#@#     }
#@#   }
#@# }
#@# //}

=== TODO ハイライト定義の注入

さて、最後に @<code>{TODO} のハイライトを定義します。
ここでは少し視点を変えて、@<b>{既存の構文定義ファイルに独自の定義を注入する方法} @<fn>{injection_grammars} を紹介します。

//footnote[injection_grammars][Syntax Highlight Guide #Injection grammars https://code.visualstudio.com/api/language-extensions/syntax-highlight-guide#injection-grammars]

==== Injection grammars

Injection grammars は、既存の構文定義ファイルのスコープに新しく定義を追加する手法です。
JSON ファイルを独自に記述し、@<code>{package.json} 上で注入するので、
既存の構文定義を汚さずに独自に拡張させることが可能となります。
ここでは一行コメントに @<code>{TODO} のハイライトを注入することにより、
どのように実装するか見ていきます。

次の @<list>{comments-line} は、 @<hd>{impl_comments} で定義したものです。
@<code>{TODO} は「@<code>{ダブルスラッシュ + TODO}」といった形で定義したいので、
この一行コメントを拡張していきます。

//list[comments-line][一行コメントの定義]{
{
  "comments-line": {
    "name": "comment.line.double-slash.abc",
    "begin": "//",
    "end": "\n",
    "patterns": [
      {
        "include": "#escape-character"
      }
    ]
  }
}
//}

==== todo-comment.injection スコープの定義

まずは @<code>{TODO} 自体のハイライトを定義していきましょう。
@<code>{./syntaxes/injection.json} ファイルを作成し、次のように記述します。

//list[injection-json][injection.json]{
{
  "scopeName": "todo-comment.injection",
  "injectionSelector": "L:comment.line.double-slash",
  "patterns": [
    {"include": "#todo-keyword"}
  ],
  "repository": {
    "todo-keyword": {
      "name": "keyword.todo",
      "match": "TODO"
    }
  }
}
//}

注入対象のスコープを @<code>{injectionSelector} に記述します。
このとき @<code>{L:} を付与することで、Inspector TM Scope で見たときに
@<code>{keyword.todo} スコープが一番上に来るようになり、 @<code>{keyword.todo} のカラーテーマが適用されるようになります。

この @<code>{todo-comment.injection} スコープを、 @<code>{package.json} の
@<code>{grammars} に次のように記述します。

//list[injection_package][todo-commentのInjection - package.json]{
{
  "grammars": [
    {
      "language": "abc",
      "scopeName": "source.abc",
      "path": "./syntaxes/abc.tmLanguage.json"
    },
    {
      "injectTo": ["source.abc"],
      "scopeName": "todo-comment.injection",
      "path": "./syntaxes/injection.json"
    }
  ]
}
//}

 * @<code>{injectTo} … 注入対象のルートスコープを指定
 * @<code>{scopeName} … 注入する独自のスコープを指定
 * @<code>{path} … 注入するスコープを記述したファイルパスを指定

これで、 @<code>{// TODO} と打つと @<code>{TODO} 部分がハイライトされると思います。
@<code>{tmLanguage.json} に触れることなくハイライトを実装することができました。

//image[todo_highlight][TODOハイライト][scale=1.0]{
//}

== まとめ

基本的なシンタックスハイライトの開発を見てきました。
VSCode の既存のカラーテーマにはさまざまなスコープと色の定義がされているので、
適切なトークンのパターンとスコープを定義すれば、比較的簡単に作成できるかと思います。

他にも、構文定義ファイルに既存の言語のスコープを埋め込む Embedded Language や、
@<code>{"include": "$self"} で再帰的に文法を適用するなど、さらに効率的に構文を定義する方法もあります。
日本語の解説記事が乏しく、Sublime Text や TextMate、VSCode の英語のドキュメントが主体となりますが、
現状のコーディング環境に不満がある場合はいろいろと試してみてはいかがでしょうか。