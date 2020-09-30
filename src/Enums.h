#pragma once

enum conflict_action
{
	share_from,
	share_to,
	copy,
	skip,
	none
};

enum load_from
{
	history,
	ini_file
};

enum sync_action
{
	update,
	upload
};
