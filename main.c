#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct	info_s
{
	int	lines;
	char	empty;
	char	obstacle;
	char	full;
	char	**map;
}		info_t;

void	prematureErause(info_t *info, size_t i)
{
	while (i--)
		free(info->map[i]);
	free(info->map);
	free(info);
	fprintf(stderr, "map error\n");
}

info_t	*mapParser(FILE* map)
{
	size_t	len = 0, cr;
	info_t	*info = calloc(1, sizeof(info_t));

	if (info == NULL)
		return (fprintf(stderr, "map error\n"), NULL);
	fscanf(map, "%d %c %c %c", &info->lines, &info->empty, &info->obstacle, &info->full);
	if (info->lines < 0 ||
		info->empty == info->obstacle || info->empty == info->full ||
		info->obstacle == info->full)
	{
		free(info);
		return (fprintf(stderr, "map error"), NULL);
	}
	info->map = calloc(info->lines, sizeof(char *));
	for (size_t i = 0; i < info->lines; i++)
	{
		cr = getline(&info->map[i], &len, map);
		if (cr < 0)
			return (prematureErause(info, i), NULL);
		if (!cr && i < info->lines)
			return (prematureErause(info, i), NULL);
	}
	return info;
}

void	freeInfo(info_t *info)
{
	while (info->lines--)
		free(info->map[info->lines]);
	free(info->map);
	free(info);
}

void	bsq(FILE* map)
{
	info_t	*info;

	if (map == NULL)
	{
		fprintf(stderr, "map error");
		return ;
	}
	info = mapParser(map);
	if (map == NULL)
	{
		fclose(map);
		return ;
	}
	freeInfo(info);
	fclose(map);
}

int	main(int ac, char **av)
{
	if (ac == 1)
		bsq(stdin);
	else
		for (size_t i = 1; i < ac; i++)
			bsq(fopen(av[i], "r"));
}
