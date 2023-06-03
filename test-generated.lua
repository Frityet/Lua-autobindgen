local test = require("test")

--function x(a: { foo: { [string] : string } }, b: string. c: { __tostring: function(): string }): string end
print(test.x({ foo = { bar = "hello " } }, "bar", "world"))

