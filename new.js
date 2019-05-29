#!/usr/bin/env node
const fs = require("fs"),
      minimist = require("minimist");

const yikes = (...a) => { console.error(...a); process.exit(1); };

const [,, ...args] = process.argv;
if (args.length < 2 || !args[1]) {
	yikes("Usage: new.js <name> [type] [-n/--namespace] [-s/--nosrc]");
}

const options = minimist(process.argv.slice(2), {
	alias: {n: "namespace", s: "nosrc"},
	boolean: ["nosrc"],
	default: {namespace: "spjalla", nosrc: false}
});

let [name, type] = options._;
if (!type) type = "core";
const {namespace, nosrc} = options;

if (!name.match(/^[\w_\d]+$/i)) {
	yikes("Invalid name:", name);
}

let allDir, sourcename, headername, sourcetext, headertext;

const sourcebase = "src", headerbase = "include";
let sourcedirs = [], headerdirs = [];

const setDirs = (...a) => { sourcedirs = [...a]; headerdirs = [...a]; };
const setNames = s => { sourcename = `${s}.cpp`; headername = `${s}.h`; };
const upper = () => name.toUpperCase();
const sourceDir = () => `${sourcebase}/${sourcedirs.join("/")}`;
const headerDir = () => `${headerbase}/${headerdirs.join("/")}`;
const sourcePath = () => `${sourceDir()}/${sourcename}`;
const headerPath = () => `${sourceDir()}/${sourcename}`;

if (type == "core") {
	setDirs("core");
	setNames(name);

	headertext = `
#ifndef CORE_${upper()}_H_
#define CORE_${upper()}_H_

namespace ${namespace} {
	
}

#endif
`.substr(1);

	sourcetext = `
#include "core/${name}.h"

namespace ${namespace} {

}
`.substr(1);

	updateModule();
} else {
	console.error("Unknown type:", type);
	yikes(`Expected "core"`);
}

if (sourcetext) write(sourcePath(), sourcetext);
if (headertext) write(headerPath(), headertext);

if (allDir) {
	const allText = read(`${headerbase}/${allDir}/all.h`);
	if (allText.indexOf(`#include "${name}.h"`) == -1) {
		write(`${headerbase}/${allDir}/all.h`, allText.replace(/(\n#endif)/, `#include "${name}.h"\n$1`));
	}
}

function updateModule() {
	const moduleText = read(`${sourceDir()}/module.mk`);
	if (moduleText.indexOf(sourcePath()) == -1) {
		write(`${sourceDir()}/module.mk`, moduleText.replace(/(#end)/, `${sourcePath()} $1\n`));
	}
}

function read(path, safe=false) {
	if (safe) {
		try {
			return read(path, false);
		} catch(e) {
			if (typeof safe == "string") {
				return `\x1b[2;31m${safe}\x1b[0m`;
			} else {
				return "";
			}
		}
	} else {
		return fs.readFileSync(path, "utf8");
	}
}

function write(path, text) {
	const old = read(path, true);
	const maxLineLength = [...text.split(/\n+/), ...old.split(/\n+/)].reduce((a, b) => Math.max(a, b.length), 0);
	const width = maxLineLength + 8;
	const padEnds = "\x1b[2m" + "".padStart(width, "━") + "\x1b[0m";
	const padMid =  "\x1b[2m" + "".padStart(width, "─") + "\x1b[0m";
	const [paddedText, paddedOld] = [text, old].map(s => "    " + s.replace(/\n/g, "\n    ").replace(/\n *$/, ""));
	console.log(`\n${padEnds}\n\x1b[1m${path + (old? "" : " (new)")}\x1b[0m\n${old? `${padMid}\n${paddedOld}\n` : ``}${padMid}\n${paddedText}\n${padEnds}\n`);
}
