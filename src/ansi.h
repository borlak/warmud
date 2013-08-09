/* ============= Ansi color table and data structure ============== */

struct ansi_color
{
	const char *symbol;		/* Symbol used in the game. eg &+symbol    */
	const char *fg_code;		/* ^[[fg_codem; = foreground color code    */
	const char *bg_code;		/* Same except it's the background color   */
} color_table[] = {

	{
		"L", "30", "40"
	},				/* Black            */
	{
		"R", "31", "41"
	},				/* Red              */
	{
		"G", "32", "42"
	},				/* Green            */
	{
		"Y", "33", "43"
	},				/* Yellow           */
	{
		"B", "34", "44"
	},				/* Blue             */
	{
		"M", "35", "45"
	},				/* Magenta          */
	{
		"C", "36", "46"
	},				/* Cyan             */
	{
		"W", "37", "47"
	},				/* White            */
	{
		NULL, NULL, NULL
	}				/* End of the table */
};

int find_color_entry(int c)
{
	int i = 0;
	char s;

	s = UPPER(c);

	while ((color_table[i].symbol != NULL) && (*color_table[i].symbol != s))
		i++;

	return i;
}
