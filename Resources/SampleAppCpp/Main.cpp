#include "Application.h"

int main(void)
{ 
	CApplication app;
	app.Create();
	while ( app() );
	app.Destroy();

	return 0;
}