#!/usr/bin/env python
# -*- coding: UTF-8 -*-

# Extract the PO template of XScreenSaver's hacks configurations.
# Run on the XScreenSaver source tree, giving target template path:
#
#    cd $SOMEWHERE/kscreensaver/kxsconfig
#    tar xzf $OTHERWHERE/xscreensaver-X.Y.tar.gz
#    ./create-hacks-pot.py xscreensaver-X.Y hacks.pot
#
# to create (i.e. update) the hacks.pot file in current working directory.
# This file should then be committed.
#
# Chusslove Illich <caslav.ilic@gmx.net>

import sys
import os
import glob
import re
import xml.parsers.expat


def main ():

    if len(sys.argv) < 2:
        error("Path to XScreenSaver source not given on command line.")
    if len(sys.argv) < 3:
        error("Path to template PO not given on command line.")
    if len(sys.argv) > 3:
        error("Too many arguments on command line.")
    xsrootdir = sys.argv[1]
    potpath = sys.argv[2]

    xscfgdir = os.path.join(xsrootdir, "hacks", "config")
    xscfgpaths = glob.glob(os.path.join(xscfgdir, "*.xml"))
    xscfgpaths.sort()

    xcfgex = XSCfgExtractor()
    xcfgex.extract(xscfgpaths, potpath, xsrootdir)


def error (msg):

    print "%s [error]: %s" % (os.path.basename(sys.argv[0]), msg)
    sys.exit(1)


class XSCfgExtractor:

    def __init__ (self):

        pass


    def _prepare_xml_parser (self, relpath):

        self._parser = xml.parsers.expat.ParserCreate()
        self._parser.UseForeignDTD() # to ignore non-default XML entities

        self._parser.StartElementHandler = self._handler_start_element
        self._parser.EndElementHandler = self._handler_end_element
        self._parser.CharacterDataHandler = self._handler_character_data

        # Data structures used in handlers
        self._element_stack = []
        self._relpath = relpath
        self._chardata = []

        return self._parser


    def extract (self, cfgpaths, potpath, srcroot=None):

        # Extract messages from all paths.
        self.msgs = []
        for cfgpath in cfgpaths:

            # Make a path relative to source root directory if possible.
            relpath = cfgpath
            if srcroot is not None and cfgpath.startswith(srcroot):
                relpath = relpath.replace(srcroot, "", 1)
                if relpath.startswith(os.path.sep):
                    relpath = relpath[len(os.path.sep):]

            # Parse file.
            p = self._prepare_xml_parser(relpath)
            fh = open(cfgpath, "r")
            p.ParseFile(fh)
            fh.close()

        # Group messages by keys, preserving order of appearance.
        msg_keys = []
        msgs_by_key = {}
        for msgctxt, msgid, path, lno in self.msgs:
            msgkey = "%s\x04%s" % (msgctxt, msgid)
            msg = msgs_by_key.get(msgkey)
            if msg is None:
                msg = (msgctxt, msgid, [])
                msgs_by_key[msgkey] = msg
                msg_keys.append(msgkey)
            msg[2].append((path, lno))

        # Format messages into lines.
        msglines = []
        for msg in [msgs_by_key[x] for x in msg_keys]:
            msgctxt, msgid, srcrefs = msg
            if srcrefs:
                srcrefstrs = ["%s:%s" % x for x in srcrefs]
                msglines.append("#: %s\n" % " ".join(srcrefstrs))
            if msgctxt is not None:
                msglines.append("msgctxt \"%s\"\n" % poescape(msgctxt))
            msglines.append("msgid \"%s\"\n" % poescape(msgid))
            msglines.append("msgstr \"\"\n")
            msglines.append("\n")

        # Write POT file.
        fh = open(potpath, "w")
        fh.writelines([x.encode("utf-8") for x in msglines])
        fh.close()


    def _handler_start_element (self, name, attrs):

        self._element_stack.append((name, attrs))
        self._startel_lno = self._parser.CurrentLineNumber

        path = self._relpath
        lno = self._startel_lno

        if 0: pass

        elif name == "screensaver":
            msgid = attrs.get("_label")
            if msgid:
                ctxt = "@item screen saver name"
                self.msgs.append((ctxt, msgid, path, lno))
            self._ssname_lno = lno
            self._ssname_pos = len(self.msgs) - 1

        elif name == "_description":
            self._chardata = []
            # Message created on end element.

        elif name == "number":
            label = attrs.get("_label")
            if label:
                ntype = attrs.get("type")
                ctxt = None
                if ntype == "slider":
                    ctxt = "@label:slider"
                elif ntype == "spinbutton":
                    ctxt = "@label:spinbox"
                self.msgs.append((ctxt, label, path, lno))
            for limattr in ("_low-label", "_high-label"):
                limlabel = attrs.get(limattr)
                if limlabel:
                    if label:
                        ctxt = "@item:inrange %s" % label
                    else:
                        ctxt = "@item:inrange"
                    self.msgs.append((ctxt, limlabel, path, lno))

        elif name == "boolean":
            label = attrs.get("_label")
            if label:
                ctxt = "@option:check"
                self.msgs.append((ctxt, label, path, lno))

        elif name == "string":
            label = attrs.get("_label")
            if label:
                ctxt = "@label:textbox"
                self.msgs.append((ctxt, label, path, lno))

        elif name == "file":
            label = attrs.get("_label")
            if label:
                ctxt = "@label:chooser"
                self.msgs.append((ctxt, label, path, lno))

        elif name == "select":
            label = attrs.get("_label")
            if label:
                ctxt = "@title:group"
                self.msgs.append((ctxt, label, path, lno))

        elif name == "option":
            label = attrs.get("_label")
            if label:
                prevlabel = self._element_stack[-2][1].get("_label")
                if prevlabel:
                    ctxt = "@option:radio %s" % prevlabel
                else:
                    ctxt = "@option:radio"
                self.msgs.append((ctxt, label, path, lno))


        #print name, attrs, self.parser.CurrentLineNumber


    def _handler_end_element (self, name):

        name, attr = self._element_stack.pop()
        path = self._relpath
        lno = self._startel_lno

        if name == "_description":
            text = "".join(self._chardata)
            rx = re.compile(r"\s+", re.U)
            text = rx.sub(" ", text.strip())
            ctxt = "@info screen saver description"
            if text:
                # Fudge position of the message,
                # to have description right after the name in the PO file.
                msg = (ctxt, text, path, self._ssname_lno)
                self.msgs.insert(self._ssname_pos + 1, msg)


    def _handler_character_data (self, text):

        self._chardata.append(text)


def poescape (text):

    text = text.replace("\"", "\\\"")
    text = text.replace("\n", "\\n")
    text = text.replace("\t", "\\t")

    return text


if __name__ == "__main__":
    main()
