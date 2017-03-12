(defun
    (HTML-mode
        (setq mode-string "HTML")
        (use-syntax-table "HTML")
        (use-local-map "HTML-map")
        (use-abbrev-table "HTML")
        (novalue)
    )
)

(defun
    (HTML-view-buffer
        (if buffer-is-modified
            (write-current-file)
        )
        (WIN-HTML-command current-buffer-file-name)
    )
)

(defun
    (HTML-lower-case-elements-and-attributes
        (save-window-excursion
            ~old-case-fold-search
            (setq ~old-case-fold-search case-fold-search)

            (beginning-of-file)
            (setq case-fold-search 0)
            (while
                (! (error-occurred (ere-search-forward "<[ \t]*[A-Z]+")))
                (progn
                    ; lower case the element name
                    (case-word-lower)
                    ; see if there are any attributes
                    (while (! (ere-looking-at "[ \t]*>|[ \t]/[ \t]*>"))
                        (ere-search-forward "[A-Za-z]+")
                        (progn
                            (case-word-lower)
                            ; make sure that its a attrib=value and not the
                            ; old style attrib on its own
                            (if (ere-looking-at "[ \t]*=")
                                (progn
                                    (ere-search-forward "=")
                                    (if (ere-looking-at "[ \t]*\"")
                                        (progn
                                            ; skip "quoted param"
                                            (ere-search-forward "\"")
                                            (ere-search-forward "\"")
                                        )
                                        (progn
                                            ; skip unquoted-param
                                            (forward-word)
                                        )
                                    )
                                )
                            )
                        )
                    )
                )
            )
            ; now deal with the closing tag
            (beginning-of-file)
            (setq case-fold-search 0)
            (while
                (! (error-occurred (ere-search-forward "< */ *[A-Z]+")))
                (case-word-lower)
            )
        )
    )
)

(defun
    (~mode-modify-syntax-table
        ~type
        ~arg
        (setq ~type (arg 1))
        (setq ~arg 2)
        (while (<= ~arg (nargs))
            (modify-syntax-table ~type (arg ~arg))
            (setq ~arg (+ ~arg 1))
        )
    )
)
(defun
    ~HTML-setup-syntax( ~syntax-table ~keyword-modifier)
    (save-window-excursion
        (temp-use-buffer "~mode-hack")
        (use-syntax-table ~syntax-table)
        (modify-syntax-table "word" "-_")

        (modify-syntax-table "comment" ">" "<")
        (modify-syntax-table "string" "\"")
        (~mode-modify-syntax-table (concat "keyword-1" ~keyword-modifier)
            "a"
            "address"
            "applet"
            "area"
            "b"
            "base"
            "basefont"
            "bgsound"
            "big"
            "blink"
            "blockquote"
            "body"
            "br"
            "button"
            "caption"
            "center"
            "cite"
            "code"
            "col"
            "colgroup"
            "comment"
            "dd"
            "dfn"
            "dir"
            "div"
            "dl"
            "dt"
            "em"
            "embed"
            "figcapture"
            "figure"
            "font"
            "form"
            "frame"
            "frameset"
            "h1"
            "h2"
            "h3"
            "h4"
            "h5"
            "h6"
            "head"
            "hr"
            "html"
            "i"
            "iframe"
            "ilayer"
            "img"
            "input"
            "isindex"
            "kbd"
            "label"
            "layer"
            "li"
            "link"
            "listing"
            "map"
            "marquee"
            "menu"
            "meta"
            "multicol"
            "nextid"
            "nobr"
            "noembed"
            "noframes"
            "noscript"
            "object"
            "ol"
            "option"
            "p"
            "param"
            "plaintext"
            "pre"
            "s"
            "samp"
            "script"
            "select"
            "server"
            "small"
            "spacer"
            "span"
            "strike"
            "strong"
            "style"
            "sub"
            "sup"
            "table"
            "tbody"
            "td"
            "textarea"
            "tfoot"
            "th"
            "thead"
            "title"
            "tr"
            "tt"
            "u"
            "ul"
            "var"
            "wbr"
            "xmp"
        )
        (~mode-modify-syntax-table (concat "keyword-2" ~keyword-modifier)
            "above="
            "accept-charset="
            "action="
            "align="
            "alink="
            "alt="
            "archive="
            "background="
            "behavious="
            "below="
            "bgcolor="
            "bgproperties="
            "border="
            "bordercolor="
            "bordercolordark="
            "bordercolorlight="
            "cellpadding="
            "cellspacing="
            "charset="
            "checked="
            "class="
            "classid="
            "clear="
            "clip="
            "code="
            "codebase="
            "codetype="
            "col="
            "color="
            "cols="
            "colspan="
            "compact="
            "content="
            "controls"
            "coords="
            "data="
            "declare"
            "direction="
            "disabled="
            "dynsrc="
            "enctype="
            "face="
            "for="
            "frame="
            "frameborder="
            "framespacing="
            "gutter="
            "height="
            "hidden="
            "href="
            "href="
            "hspace="
            "http-equiv="
            "id="
            "ismap="
            "lang="
            "language="
            "left="
            "leftmargin="
            "link="
            "loop="
            "lowsrc="
            "marginheight="
            "marginwidth="
            "maxlength="
            "mayscript="
            "method="
            "multiple"
            "n="
            "name="
            "nohref="
            "noresize="
            "noshade="
            "notab="
            "nowrap="
            "onabort="
            "onblur="
            "onchange="
            "onclick="
            "onerror="
            "onfocus="
            "onload="
            "onload="
            "onmouseout="
            "onmouseover="
            "onreset="
            "onselect="
            "onsubmit="
            "onunload="
            "palette="
            "pluginspage="
            "prompt="
            "rel="
            "rev="
            "rows="
            "rowspan="
            "rules="
            "scrollamount="
            "scrolldelay="
            "scrolling="
            "selected"
            "shape="
            "shapes"
            "size="
            "span="
            "src="
            "standby="
            "start="
            "style="
            "tabindex="
            "taborder="
            "target="
            "text="
            "title="
            "top="
            "topmargin="
            "type="
            "type="
            "type="
            "units="
            "usemap="
            "usemap="
            "valign="
            "value="
            "valuetype="
            "version="
            "visility="
            "vlink="
            "vspace="
            "vspace="
            "width="
            "wrap="
            "z-index="
        )
    )
)
(defun 
    (~HTML-setup-keymap
        (temp-use-buffer "~mode-hack")

        (define-keymap "HTML-map")
        (use-local-map "HTML-map")

        (local-bind-to-key "HTML-view-buffer" "\ec")

        (kill-buffer "~mode-hack")
        (novalue)
    )
)
(~HTML-setup-syntax "HTML" ",case-fold")
(~HTML-setup-keymap)
