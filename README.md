# escalation_filter Groonga function

* ``escalation_filter("script_expr"[, threshold])``

関数が呼ばれたタイミングで結果テーブルの件数がthreshold以上の場合にのみfilterを実行する。

threshold: デフォルト0


## Install

Install libgroonga-dev.

Build this function.

    % sh autogen.sh
    % ./configure
    % make
    % sudo make install

## Usage

Register `functions/escalation_filter`:

    % groonga DB
    > register functions/escalation_filter

## Author

Naoya Murakami naoya@createfield.com

## License

LGPL 2.1. See COPYING-LGPL-2.1 for details.
