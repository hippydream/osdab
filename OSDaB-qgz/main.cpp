#include <QApplication>
#include <QFileDialog>
#include <QDataStream>
#include <QMessageBox>
#include "zlib/zlib.h"

#define UNZIP_READ_BUFFER (256*1024)

bool inflate(const QString& s);
QString decompressedFileName;

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	QString s = QFileDialog::getOpenFileName(0, "Open gzip file", "", "");
	if (s.isEmpty())
		return 0;

	bool res = inflate(s);
	if (res) {
		if (decompressedFileName.isEmpty())
			QMessageBox::information(0, "OSDaB qgz", QLatin1String("Decompression completed with no errors."));
		else QMessageBox::information(0, "OSDaB qgz", QString("Decompression completed with no errors.\n Output written to: \n").append(decompressedFileName));
	}
	else QMessageBox::warning(0, "OSDaB qgz", QLatin1String("Sorry but decompression failed."));

	return 0;
}

bool inflate(const QString& s)
{
	QFile file(s);
	if (!file.open(QIODevice::ReadOnly)) {
		qDebug("Failed to open input file: %s", qPrintable(s));
		return false;
	}

	// Output to a QByteArray....
	//
	// QByteArray decompressedData;
	// QDataStream dev(&decompressedData, QIODevice::WriteOnly);
	//
	// ...or to a QFile
	//
	QFileInfo info(s);
	QString outFileName;
	if (info.suffix() == "gz") {
		outFileName = info.absoluteFilePath().left( info.absoluteFilePath().length() - 3 );
	} else if (info.suffix() == "svgz") {
		outFileName = info.absoluteFilePath().left( info.absoluteFilePath().length() - 1 );
	} else {
		outFileName = info.absoluteFilePath().append(".decompressed");
	}
	// Quick and dirty :D
	int magik = 0;
	while (QFile::exists(outFileName)) {
		outFileName.append(QString(".%1").arg(++magik));
	}
	QFile out(outFileName);
	if (!out.open(QIODevice::WriteOnly)) {
		qDebug("Failed to open output file: %s", qPrintable(outFileName));
		return false;
	}
	decompressedFileName = outFileName;
	QDataStream dev(&out);

	quint64 compressedSize = file.size();

	uInt rep = compressedSize / UNZIP_READ_BUFFER;
	uInt rem = compressedSize % UNZIP_READ_BUFFER;
	uInt cur = 0;

	// extract data
	qint64 read;
	quint64 tot = 0;

	char buffer1[UNZIP_READ_BUFFER];
	char buffer2[UNZIP_READ_BUFFER];

	/* Allocate inflate state */
	z_stream zstr;
	zstr.zalloc = Z_NULL;
	zstr.zfree = Z_NULL;
	zstr.opaque = Z_NULL;
	zstr.next_in = Z_NULL;
	zstr.avail_in = 0;

	int zret;

	/*
	 windowBits can also be greater than 15 for optional gzip decoding. Add
	32 to windowBits to enable zlib and gzip decoding with automatic header
	detection, or add 16 to decode only the gzip format (the zlib format will
	return a Z_DATA_ERROR.  If a gzip stream is being decoded, strm->adler is
	a crc32 instead of an adler32.
	*/
	if ( (zret = inflateInit2_(&zstr, MAX_WBITS + 16, ZLIB_VERSION, sizeof(z_stream))) != Z_OK ) {
		qDebug("Failed to initialize zlib");
		return false;
	}

	int szDecomp;

	// Decompress until deflate stream ends or end of file
	do
	{
		read = file.read(buffer1, cur < rep ? UNZIP_READ_BUFFER : rem);
		if (read == 0)
			break;
		if (read < 0)
		{
			(void)inflateEnd(&zstr);
			qDebug("Read error");
			return false;
		}

		cur++;
		tot += read;

		zstr.avail_in = (uInt) read;
		zstr.next_in = (Bytef*) buffer1;


		// Run inflate() on input until output buffer not full
		do {
			zstr.avail_out = UNZIP_READ_BUFFER;
			zstr.next_out = (Bytef*) buffer2;;

			zret = inflate(&zstr, Z_NO_FLUSH);

			switch (zret) {
				case Z_NEED_DICT:
				case Z_DATA_ERROR:
				case Z_MEM_ERROR:
					inflateEnd(&zstr);
					qDebug("zlib failed to decode file");
					return false;
				default:
					;
			}

			szDecomp = UNZIP_READ_BUFFER - zstr.avail_out;
			if (dev.writeRawData(buffer2, szDecomp) != szDecomp)
			{
				inflateEnd(&zstr);
				qDebug("Write error");
				return false;
			}

		} while (zstr.avail_out == 0);

	}
	while (zret != Z_STREAM_END);

	inflateEnd(&zstr);

	return true;
}
