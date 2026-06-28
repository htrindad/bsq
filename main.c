#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct	info_s
{
	int	lines;
	int	cols;
	char	empty;
	char	obstacle;
	char	full;
	char	**map;
}		info_t;

typedef struct	potential_s
{
	size_t	y;
	size_t	x;
	size_t	size;
}		potential_t;

void	prematureErause(info_t *info, size_t i)
{
	while (i--)
		free(info->map[i]);
	free(info->map);
	free(info);
	fprintf(stderr, "map error\n");
}

info_t	*mapParser(FILE *map)
{
	info_t	*info = calloc(1, sizeof(info_t));

	if (info == NULL)
		return (fprintf(stderr, "map error\n"), NULL);
	if (fscanf(map, "%d %c %c %c", &info->lines, &info->empty,
			&info->obstacle, &info->full) != 4
		|| info->lines <= 0
		|| info->empty == info->obstacle
		|| info->empty == info->full
		|| info->obstacle == info->full)
	{
		free(info);
		return (fprintf(stderr, "map error\n"), NULL);
	}
	else
	{
		char	*tmp = NULL;
		size_t	n = 0;
		getline(&tmp, &n, map);
		free(tmp);
	}
	info->map = calloc(info->lines, sizeof(char *));
	if (info->map == NULL)
	{
		free(info);
		return (fprintf(stderr, "map error\n"), NULL);
	}
	for (int i = 0; i < info->lines; i++)
	{
		size_t	len = 0;
		long	cr = getline(&info->map[i], &len, map);
		if (cr <= 0)
			return (prematureErause(info, i), NULL);
		int	line_cols = (int)cr;
		if (info->map[i][line_cols - 1] == '\n')
			line_cols--; // removing the new line
		if (!line_cols)
			return (prematureErause(info, i + 1), NULL);
		if (!i)
			info->cols = line_cols;
		else if (line_cols != info->cols)
			return (prematureErause(info, i + 1), NULL);
		for (int j = 0; j < line_cols; j++)
			if (info->map[i][j] != info->empty && info->map[i][j] != info->obstacle) // verification
				return (prematureErause(info, i + 1), NULL);
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

potential_t	potentialConstructor()
{
	potential_t	potential;

	potential.size = 0;
	potential.y = 0;
	potential.x = 0;
	return potential;
}

void	testPotential(potential_t *potential, info_t const info)
{
	size_t	y = potential->y;
	size_t	x = potential->x;
	size_t	size = 0;
	int		fits;

	while (true)
	{
		size_t	s = size + 1;
		if ((int)(y + s) > info.lines || (int)(x + s) > info.cols)
			break ;
		fits = 1;
		for (size_t i = x; i < x + s && fits; i++)
			if (info.map[y + s - 1][i] == info.obstacle) // check with the size of the current iteration in the vertical lining
				fits = 0;
		for (size_t i = y; i < y + s - 1 && fits; i++)
			if (info.map[i][x + s - 1] == info.obstacle) // check with the size of the current iteration on the horizontal lining
				fits = 0;
		if (!fits)
			break ;
		size = s;
	}
	potential->size = size;
}

potential_t	copyPotential(potential_t const cpy)
{
	potential_t	new;

	new.size = cpy.size;
	new.x = cpy.x;
	new.y = cpy.y;
	return new;
}

void	findSquare(info_t const info)
{
	potential_t	currentPotential = potentialConstructor();
	potential_t	newPotential = potentialConstructor();

	for (size_t y = 0; y < (size_t)info.lines; y++)
	{
		for (size_t x = 0; x < (size_t)info.cols; x++)
		{
			if (info.map[y][x] == info.empty)
			{
				newPotential.y = y; newPotential.x = x;
				testPotential(&newPotential, info);
				if (newPotential.size > currentPotential.size)
					currentPotential = copyPotential(newPotential);
			}
		}
	}
	for (size_t y = currentPotential.y; y < currentPotential.y + currentPotential.size; y++)
		for (size_t x = currentPotential.x; x < currentPotential.x + currentPotential.size; x++)
			info.map[y][x] = info.full;
	for (int i = 0; i < info.lines; i++)
		fprintf(stdout, "%s", info.map[i]);
	//fputs(info.map[i], stdout);
}

void	bsq(FILE *map, int sep)
{
	info_t	*info;

	if (map == NULL)
	{
		fprintf(stderr, "map error\n");
		if (sep)
			fputs("\n", stdout);
		return ;
	}
	info = mapParser(map);
	if (map != stdin)
		fclose(map);
	if (info == NULL)
	{
		if (sep)
			fputs("\n", stdout);
		return ;
	}
	findSquare(*info);
	freeInfo(info);
	if (sep)
		fputs("\n", stdout);
}

int	main(int ac, char **av)
{
	int	sep = ac > 2;

	if (ac == 1)
		bsq(stdin, 0);
	else
		for (int i = 1; i < ac; i++)
			bsq(fopen(av[i], "r"), sep);
	return 0;
}
