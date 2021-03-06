; Copyright (c) 2017 Quenza Inc. All rights reserved.
; Copyright (c) 2018 Blub Corp. All Rights Reserved.
;
; This file is part of the Cryptox project.
;
; Use of this source code is governed by a private license
; that can be found in the LICENSE file. Content can not be 
; copied and/or distributed without the express of the author.

define _DEFAULT_SPEC;

; Set the default target
default_target aiipx;
;default_target casm;

env {
    auto_capture on;
};

directory {
    include   "\\include";
    extension "\\ext";
};

; Safety options
defensive         on;
flow_guard        on;
sanitize_section  off;

; Resulting image options
keep_comment      off;
string_squashing  on;
dump_section      off;
keep_zero_ref_cnt off;

; Image output format
default_extension "cex"
output_format "$(filename).$(default_extension)"

; Log build output to file
build_output "$(filename).$(timestamp).log"

; Compiler extension support
extension {
    builtin_cil     on;
    allow_external  on;
    autoload_all    on;
    require_signed  on;

    ; whitelist {
    ;  Add specific extensions to autoload at start
    ; };
};

; Development options
safe_mode         off;
debug_level       0
