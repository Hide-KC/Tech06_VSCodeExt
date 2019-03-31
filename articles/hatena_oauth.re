={hatena_oauth} はてなブログの OAuth 認証を通す

ある日、はてなブログで記事を書いていて、ふと思いました。

//quote{
「ハイライト効かないし、インデントとかもやりづらいし、はてなブログに VSCode から直接投稿したいンゴ……」
//}

調べると、はてなブログではサードパーティ製アプリのための OAuth 認証が用意されています。 @<fn>{hatena_dev}
また、すでに VSCode から投稿できるようになる拡張機能はありましたが、それは OAuth 認証を使用していないようでした。
@<br>{}

@<b>{……試さない手はないですよね？}
@<br>{}

ということで、やってみました。

//footnote[whats_oauth][OAuth 認証の認可フローについては各自で調査してください＞＜]
//footnote[hatena_dev][Consumer key を取得して OAuth 開発をはじめよう http://developer.hatena.ne.jp/ja/documents/auth/apis/oauth/consumer]

== 拡張機能の要件

 * コマンドで OAuth 認証を開始する
 * アクセストークン取得まで、可能な限り選択肢を表示しない
 * ブラウザを経由しない

規約上は、はてなアカウントの権限取得時に Yes/No の選択肢を明示しなければなりませんが、完全個人利用なのでカットしています。
そのため本実装では、コマンド実行後アクセストークンの取得まで選択肢は一切表示されません。

=={oauth_flow} アクセストークン取得までの流れ

//list[oauth_flow][アクセストークン取得までのフロー]{
（コマンド実行）
　↓
リクエストトークンの取得
　↓
はてなへのログイン処理（Cookie の取得）
　↓
認可ページにリダイレクト
　↓
権限取得の承認（OAuth Verifier の取得）
　↓
アクセストークンの取得
　↓
（完了）
//}

== 必要な Node モジュール

 * request-promise
 ** HTTP リクエストに利用
 * request
 ** request-promise を使うために必要
 * oauth
 ** OAuth 認証に利用

request モジュールを直接利用してもいいですが、プチコールバック地獄になるので
request-promise を使用したほうがスッキリすると思います。
また、oauth モジュールはコールバックベースなので、必要なメソッドを Promise でラップします。

== 実装

さすがにコード全部は載せられないので、本片手にこちらのリポジトリをご覧ください。

 * HatenaBlogCode https://github.com/Hide-KC/HatenaBlogCode

認証は @<code>{src/Authorizer.ts} の @<code>{startOAuth} メソッドにまとめてあります。
かいつまんで説明していきます。

=== oauth モジュールのメソッドを Promise でラップ

次のメソッドを認証時にコールするので、Promise でラップします。

 * getOAuthRequestToken - リクエストトークンの取得
 * getOAuthAccessToken - アクセストークンの取得

//listnum[get_request][getOAuthAccessToken]{
const getOAuthRequestToken = (scope: {[key: string]: string}) => 
  new Promise<{...}>((resolve) =>
    this.oauth.getOAuthRequestToken(scope, 
      (err, requestToken, requestTokenSecret, query) => {
        resolve({err, requestToken, requestTokenSecret, query});
      })); 
//}

//listnum[get_access][getOAuthAccessToken]{
const getOAuthAccessToken = 
  (requestToken: string, requestTokenSecret: string, verifier: string) => 
    new Promise<{...}>((resolve) => 
      this.oauth.getOAuthAccessToken(
        requestToken, requestTokenSecret, verifier,
          (err, accessToken, accessTokenSecret, query) => {
        resolve({err, accessToken, accessTokenSecret, query});
      }));
//}


返ってくる Promise の型は、@<code>{oauth} モジュールの @<code>{oauth1tokenCallback} と同じ
@<code>{{err: {statusCode: number, data?: any\}, requestToken: string, requestTokenSecret: string, query: any\}}
としました。リスト中では長すぎるので省略しています（@<code>{Promise<{...\}>}の部分）。

Promise のラップって最初はよく分からなかったのですが、 @<code>{then} で記述する匿名関数が
@<code>{resolve} に置き換わると考えたらなんとなく理解できました（たぶん）。

=== 認可フローを async/await で実装する

@<list>{oauth_flow}を実装します。

//listnum[promise_flow][認可フローの実装]{
try {
  // リクエストトークンの取得
  const _requestTokenResult = getOAuthRequestToken({
    'scope': 'read_public,write_public,read_private,write_private'
  });
  // Cookieの取得
  const _rk = this.getRK();

  const requestTokenResult = await _requestTokenResult;
  const rk = (await _rk) as string;

  // RKMの取得
  const rkm = this.getRKM(requestTokenResult.requestToken, rk);
  
  // OAuth Verifierの取得
  const verifier =
    this.getVerifier(requestTokenResult.requestToken, rk, (await rkm) as string);
  
  // アクセストークンの取得
  const accessTokenResult =
    await getOAuthAccessToken(
      requestTokenResult.requestToken,
      requestTokenResult.requestTokenSecret,
      (await verifier) as string
    );
  
  console.log(">>>Congraturations!!<<<");
  console.log('AccessToken: ' + accessTokenResult.accessToken);
  console.log('AccessTokenSecret: ' + accessTokenResult.accessTokenSecret);
  ...
} catch (reason) {
  console.log(reason);
  ...
}
//}

はてなブログに記事を投稿する場合、権限は次の４つが必要です。

 * read_public, write_public, read_private, write_private

権限を変えるとアクセストークンも変わるので、間違えないようにしましょう。
以下、途中の @<code>{this.get~} メソッドについて記述します。

==== getRK メソッド

はてなのログイン URL にユーザ ID 等を @<code>{POST} し、ログイン時の Cookie を取得します。
これがないと、認証ページをリクエストした際に次のようなページが返ってきてしまい、
処理がややこしくなってしまいます。 @<fn>{touzen_http}

//footnote[touzen_http][画像は例としてChromeで開いたものです。これらのページの HTML が返ってきます]

//image[login_hikaku][ログイン状態で認証ページを開く][scale=0.72]{
//}

//list[match_rk][RKの取得]{
const cookie = response.headers['set-cookie'];
if (cookie !== undefined){
  const _rk = (cookie as string[])[5].match("(rk=.*); domain");
  ...
//}

正規表現で強引に取得しましょう（以下同様）。

==== getRKM メソッド

@<img>{login_hikaku} の右側のページが返ってきた際に、「許可」ボタンを押すためのキー RKM を取得します。@<fn>{python_arigatou}
RKM を次のステップの @<code>{getVerifier} に渡して @<code>{POST} し、「許可」ボタン押下相当の処理を行います。

//list[match_rkm][RKMの取得]{
const _rkm = response.match("name=\"rkm\" value=\"(.*)\"");
//}

//footnote[python_arigatou][Pythonで実装されていた方のコードが大変参考になりました https://github.com/iruca/hatena-oauth-python/blob/master/get_access_token_util.py]

==== getVerifier メソッド

「許可」ボタン押下相当の処理を行い、返ってきた HTML から @<code>{verifier} を取得します。

//list[match_verifier][Verifierの取得]{
const _verifier = response.match("<div class=verifier><pre>(.*)</pre></div>");
//}

@<code>{verifier} を次の @<code>{getOAuthAccessToken} に渡すことで、
めでたくアクセストークンの取得が完了します。

== コマンドとして追加する

危うく忘れるところでした。
@<code>{registerCommand} でコマンドを追加しましょう。
コマンドID @<code>{'extension.startOAuth'} を @<code>{package.json} に定義するのを忘れないようにしてください。

//listnum[register][registerCommand - extension.ts]{

export function activate(context: vscode.ExtensionContext) {
  //Start Authorize
  const startOauth = () => {
    Authorizer.getInstance().startOAuth();
  };
  context.subscriptions.push(
    vscode.commands.registerCommand('extension.startOAuth', startOauth)
  );
}
//}

== 実行したときのログ

こんな感じで取得できます。

//image[success_log][認証成功時のログ][scale=1.0]{
//}

== まとめ

完全に個人的なメモですね←

個人的に詰まったのが「ログイン状態の取得」と「許可動作」です。
わずかにネット上にヒントがあったのでなんとか実装できました。
グーグル先生はすごいぞ。

また、現在上げているコードでは、認証をシングルトンオブジェクトに担当させ、
さらに@<code>{oauth}モジュールに依存しきっています。
@<code>{IAuthorize}等の認証のインターフェースを用意して分離したほうがいいですね。

あと認証には関係ありませんが、Promise、これはやっかいでした。
非同期処理は Android で経験ありますが、Promise の理解にだいぶ時間をとられてしまいました。
最初は @<code>{Promise#then} でチェーンを構築していましたが、@<code>{async/await} の方がずっと簡単なのではないかなーと思います。
@<br>{}

記事の投稿可能な状態まで拡張機能は作りこんでありますが、
承認の許可・拒否関係をまるごとすっ飛ばしているのでマーケットプレイスには公開できません。
完全自己責任案件にはなりますが、万が一個人的に利用したいという方がいたらご連絡ください。VSIX ファイルにて配布します。