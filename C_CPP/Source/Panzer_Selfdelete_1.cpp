/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <windows.h>
#include <stdio.h>
#include "Panzer_Definitions.h"




void deleteYourself()
{
  char lCommand[MAX_BUF_SIZE + 1];
  char lFile[MAX_BUF_SIZE + 1];


  ZeroMemory(lCommand, sizeof(lCommand));

  if((GetModuleFileName(0, lFile, sizeof(lFile)) != 0) && (GetShortPathName(lFile, lFile, MAX_BUF_SIZE) != 0))
  {
    _snprintf(lCommand, sizeof(lCommand) - 1, "/c del %s >> NUL", lFile);
    GetEnvironmentVariable("ComSpec", lFile, MAX_PATH) && ShellExecute(0, 0, lFile, lCommand, 0, SW_HIDE);
  }// else
}