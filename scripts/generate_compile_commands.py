Import("env")

# This script will generate a compile_commands.json file for clang-tidy
env.Execute("$PYTHONEXE -m platformio run --target compiledb")
