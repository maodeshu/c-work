#include<SFML/Graphics.hpp>//����ͼ���ͷ�ļ�
#include<SFML/Audio.hpp>//���������ļ�
#include<time.h>//����ʱ���ļ�
using namespace sf;
//��ת���������ӷ֣�����
const int ROW_COUNT = 20;
const int COL_COUNT = 10;
//�½��ٶ�
const float SPEED_NORMAL = 0.3;
const float SPEED_QUICK = 0.05;
float delay = SPEED_NORMAL;

int blockIndex;//��ǰ���������
int blocks[7][4] = {//�������ַ��������
	{1,3,5,7},//I
	{2,4,5,7},//Z
	{3,5,4,6},//Z
	{3,5,4,7},//T
	{2,3,5,7},//L
	{3,5,7,6},//J
	{2,3,4,5}//��
};
int table[ROW_COUNT][COL_COUNT] = { 0 };//��Ϸ����ı�ʾ
				  //��table[i][j]=0���±�Ϊij�Ŀհף�����ֵ��ʾ��ɫȡֵ
struct Point {//����˹����ı�ʾ������4С��ģ�λ�ò��Ϸ����仯

	int x;
	int y;
} curBlock[4],bakBlock[4];//����һ������block

Sound sou;//����������ʾ
//�÷���ʾ
Font font;
Text textScore;
int score = 0;

bool check() {//����ǲ��Ǳ�д
	for (int i = 0; i < 4; i++) {
		if (curBlock[i].x < 0 || curBlock[i].x >= COL_COUNT || curBlock[i].y >= ROW_COUNT || curBlock[i].y <= 0 || table[curBlock[i].y][curBlock[i].x])	//�����߽�//�᲻���ظ�λ��
			//�����߽�//�᲻���ظ�λ��
		{
			return false;
		}
	}
	return true;
}

void moveLeftRight(int dx) {
	for (int i = 0; i < 4; i++) {
		bakBlock[i] = curBlock[i];//����
		curBlock[i].x += dx;
	}
	if (!check()) {
		for (int i = 0; i < 4; i++) {
			curBlock[i] = bakBlock[i];//����
		}
	}
}
//1.�ռ任ʱ�䣬��������̬������
//�ٶȿ죬ռ���ڴ��
//2.�㷨������ת
void doRotate() {
	if (blockIndex == 7) {  // �����Σ�����Ҫ��ת
		return;
	}
	//���ݵ�ǰ���飬�������ٻ���
	for (int i = 0; i < 4; i++) {
		bakBlock[i] = curBlock[i];  
	}

	Point p = curBlock[1]; //��ת����
	//a[i].x=p.x-a[i].y+p.x
	//a[i].y=p.y+a[i].x-p.x;
	for (int i = 0; i < 4; i++)
	{
		struct Point tmp = curBlock[i];
		curBlock[i].x = p.x - tmp.y + p.y;
		curBlock[i].y = p.y + tmp.x - p.x;
	}
	if (!check()) {
		for (int i = 0; i < 4; i++) {
			curBlock[i] = bakBlock[i];
		}
	}
}
//���ת��
void keyEvent(RenderWindow *window) {//������Ӧ
	bool rotate = false;//�Ƿ���ת
	int dx = 0;//ƫ����
	Event e;//�¼�����
	//pollEvent �Ӷ������淵��һ���¼�
	//���¼� false
	while (window->pollEvent(e)) {
		if (e.type == Event::Closed)//�¼��ǹرմ���
		{
			window->close();
		}
		if (e.type == Event::KeyPressed)
		{
			switch (e.key.code) {
			case Keyboard::Up:
				rotate = true;
				break;
			case Keyboard::Left:
				dx = -1;
				break;
			case Keyboard::Right:
				dx = 1;
				break;
			default:
				break;
			}
		}
		//һֱ�½�
		if (Keyboard::isKeyPressed(Keyboard::Down)) {
			delay = SPEED_QUICK;//����
		}

		if (dx != 0) {
			moveLeftRight(dx);
		}

		//��ת����
		if (rotate) {
			doRotate();
		}
	}
}

void newBlock() {//���ɷ���

	//�������
	blockIndex = 1 + rand() % 7;
	int n = blockIndex - 1;
	for (int i = 0; i < 4; i++) {
		curBlock[i].x = blocks[n][i] % 2;
		curBlock[i].y = blocks[n][i] / 2;
	}
}

void drawBlocks(RenderWindow* window, Sprite* spriteBlock) {
	// �����ѽ�����ϵķ���
	for (int i = 0; i < ROW_COUNT; i++)
		for (int j = 0; j < COL_COUNT; j++)
		{
			if (table[i][j] == 0) continue;
			spriteBlock->setTextureRect(IntRect(table[i][j] * 18, 0, 18, 18));
			spriteBlock->setPosition(j * 18, i * 18);
			spriteBlock->move(28, 31); //�����߿�λ��
			window->draw(*spriteBlock);
		}
	// ���Ƶ�ǰ����
	for (int i = 0; i < 4; i++)
	{
		spriteBlock->setTextureRect(IntRect(blockIndex * 18, 0, 18, 18));
		spriteBlock->setPosition(curBlock[i].x * 18, curBlock[i].y * 18);//�����и�һ��С����
		spriteBlock->move(28, 31); //�����߿�λ��
		window->draw(*spriteBlock);
	}
}
void drop() {
	//y�����һ�Ϳ���
	for (int i = 0; i < 4; i++) {
		bakBlock[i] = curBlock[i];
		curBlock[i].y += 1;
	}
	//���ܴ�������
	if (check() == false) {
		//�̶�����
		for (int i = 0; i < 4; i++) {
			table[bakBlock[i].y][bakBlock[i].x] = blockIndex;
		}
		//�����·���
		newBlock();
	}
}
void clearLine() {
	int k = ROW_COUNT - 1;//����д����
	for (int i = ROW_COUNT - 1; i > 0; i--) {
		int count = 0;
		for (int j = 0; j < COL_COUNT; j++) {
			if (table[i][j]) {
				count++;
			}
			table[k][j] = table[i][j];//һ��ͳ��һ��дһ��
		}
		if (count < COL_COUNT) {
			k--;//��ȥ����
		}
		else {
			score += 10;
			sou.play();//��������Ч
		}
	}
	//��ʾ����
	char tmp[16];
	sprintf_s(tmp, "%d", score);
	textScore.setString(tmp);
}
void initScore() {
	if (!font.loadFromFile("C:/Users/Hey/Desktop/����˹����/ConsoleApplication1/Sansation.ttf")) {
		exit(1);
	}

	textScore.setFont(font); // font is a sf::Font
	textScore.setCharacterSize(30);// set the character size
	textScore.setFillColor(sf::Color::Black); // set the color
	textScore.setStyle(sf::Text::Bold); // set the text style
	textScore.setPosition(255, 175);
	textScore.setString("0");
}

int main(void) {

	//��Ϸ������
	Music music;
	if (!music.openFromFile("C:/Users/Hey/Desktop/����˹����/ConsoleApplication1/bg2.wav"))
	{
		return -1;
	}
	music.setLoop(true);//ѭ������
	music.play();

	SoundBuffer xiaochu;
	if(!xiaochu.loadFromFile("C:/Users/Hey/Desktop/����˹����/ConsoleApplication1/bg.wav")){
		return -1;
	}

	srand(time(0));//�õ�ǰʱ�������������
	RenderWindow window(VideoMode(320, 416),"HEY");//��������

	Texture bg;//���ر���ͼƬ
	bg.loadFromFile("C:/Users/Hey/Desktop/����˹����/ConsoleApplication1/image/bg.jpg");
	Sprite spriteBg(bg);//����ͼƬ�������

	Texture tile;//��С����ͼƬ
	tile.loadFromFile("C:/Users/Hey/Desktop/����˹����/ConsoleApplication1/image/tiles.png");
	Sprite spriteBlock(tile);//����ͼƬ�������
	
	Texture bk;//��С����ͼƬ
	bk.loadFromFile("C:/Users/Hey/Desktop/����˹����/ConsoleApplication1/image/bk.png");
	Sprite spriteFrame(bk);//����ͼƬ�������
	//window.draw(spriteBg);
	//window.display();//
	initScore();

	//���ɷ���
	newBlock();
	//���ü�ʱ��
	Clock clock;
	float timer = 0;

	//������Ϸѭ��
	//������Ϸ
	while (window.isOpen()) {//�������û�б��ر�

		//���������ڵ�ʱ��
		float time = clock.getElapsedTime().asSeconds();
		clock.restart(); 
		timer += time;
		
		//�ȴ��û�����
		keyEvent(&window);//�����ƶ�����ת

		if (timer > delay)
		{
			//����
			drop();//�½�һ��λ��
			timer = 0;
		}
		for (int i = 0; i < 10; i++) {
			if (table[0][i]||table[1][0]|| table[1][1] || table[2][0]||table[2][1]||table[3][0]||table[3][1] ) {
				printf("��Ϸ���������յ÷�Ϊ%d\n", score);
				system("pause");
				return -1;
			}
		}
		//���ִ���
		clearLine();
		delay = SPEED_NORMAL;//�ٶȻ�ԭ
		window.draw(spriteBg);
		window.draw(spriteFrame);
		//���Ʒ��飬������Ϸ
		drawBlocks(&window,&spriteBlock);
		//��Ⱦ����
		window.draw(textScore);//��ʾ����
		window.display();//
	}
	system("pause");
	return 0;
}