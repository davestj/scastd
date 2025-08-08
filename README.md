/* Scast Daemon 
Authors: oddsock, dstjohn
This program is free software; you can
redistribute it and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at your option) any later
version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details. You should have received a copy of the GNU General
Public License along with this program; if not, write to the Free Software Foundation, Inc., 59
Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

Configuration
-------------
scastd reads database settings from a simple configuration file at
startup. The file contains lines of the form `key value` and supports
`#` comments. By default the daemon looks for `scastd.conf` in the
current directory, but an alternate path may be supplied as the first
command line argument.

Example `scastd.conf`:

```
# database credentials
username root
password secret
```
