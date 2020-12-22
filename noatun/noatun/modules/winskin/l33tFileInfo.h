class l33tFileInfo {

public:
        l33tFileInfo(QString fileName);
        int getBps();
	int getKHz();
	QString getChannelMode();

private:
        char c[4];
	int truebits[4];
	int getbits(unsigned char x, int p, int n, int *result);
};
