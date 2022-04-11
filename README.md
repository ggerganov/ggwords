# ggwords

Generate n-gram statistics by processing the contents of English books/texts.

## Usage

```bash
git clone https://github.com/ggerganov/ggwords
cd ggwords
mkdir build
cd build
cmake ..
make -j4

./bin/analyze /path/to/metadata/books.txt /path/to/books/text
```
