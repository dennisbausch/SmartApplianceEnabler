# Logging

## Log-Dateien

Für den *Smart Appliance Enabler* sind folgende Log-Dateien relevant:
- `/tmp/rolling-<datum>.log` (z.B. `/tmp/rolling-2019-12-30.log`) enthält die Log-Ausgaben des *Smart Appliance Enabler*
- `/var/log/smartapplianceenabler.log` enthält die `stdout`-Ausgaben des *Smart Appliance Enabler*-Prozesses
- `/var/log/smartapplianceenabler.err` enthält die `stderr`-Ausgaben des *Smart Appliance Enabler*-Prozesses

## Log-Ausgaben des *Smart Appliance Enabler*

Das Logging des *Smart Appliance Enabler* wurde so implementiert, dass es transparent macht, was der *Smart Appliance Enabler* gerade macht. Oft werden auch Werte ausgegeben, bevor sie für Entscheidungen herangezogen werden. Dadurch sollte das Log auch  bestmöglich unterstützen bei einer Fehlersuche.

Soweit sich die Log-Einträge auf ein bestimmtes Gerät beziehen, enthalten diese immer die [`ID`](Appliance_DE.md#id) um das Filtern der Log-Einträge zu ermöglichen:
```
2019-12-30 11:46:51,501 DEBUG [Timer-0] d.a.s.h.HttpTransactionExecutor [HttpTransactionExecutor.java:160] F-00000001-000000000015-00: Response code is 200
```

## Log-Level

Der *Log-Level* steht standardmässig auf `debug`, um im Fehlerfall detaillierte Informationen zu haben. Falls weniger geloggt werden soll, kann der Log-Level auf `info` geändert werden in der Datei `/opt/sae/logback-spring.xml`:
```
<logger name="de.avanux" level="debug" additivity="false">
```

## Aufbewahrung der Log-Dateien

Standardmässig werden nur die Logs der letzten 7 Tage gespeichert. Bei Bedarf kann das geändert werden in der Datei `/opt/sae/logback-spring.xml`:
```
<maxHistory>7</maxHistory>
```

## Hilfreiche Befehle zur Log-Analyse

Für die Log-Analyse haben sich einige Befehle als sehr hilfreich erwiesen.

Bei den beispielhaft gezeigten Befehlen muss das Datum im Dateinamen der Log-Datei immer angepasst werden!

### Log live verfolgen
Mit dem folgenden Befehl werden kontinuierlich Log-Ausgaben für alle Geräte angezeigt, sobald sie in die Log-Datei geschrieben werden: 
```console
sae@raspi ~ $ tail -f /tmp/rolling-2020-12-31.log
2020-12-31 11:23:19,671 DEBUG [pi4j-gpio-event-executor-94] d.a.s.c.StartingCurrentSwitch [StartingCurrentSwitch.java:325] F-00000001-000000000012-00: Finished current not detected.
2020-12-31 11:23:19,683 DEBUG [Timer-0] d.a.s.u.GuardedTimerTask [GuardedTimerTask.java:54] F-00000001-000000000004-00: Executing timer task name=PollEnergyMeter id=21173788
2020-12-31 11:23:19,690 DEBUG [Timer-0] d.a.s.u.GuardedTimerTask [GuardedTimerTask.java:54] F-00000001-000000000005-00: Executing timer task name=PollEnergyMeter id=24264618
```

Wenn man diverse Geräte konfiguriert hat, ist diese Ausgabe recht unübersichtlich. In diesem Fall ist es sinnvoll, die Log-Ausgaben zu filtern, sodass man nur diejenigen des interssierenden Gerätes zu sehen bekommt:
```console
pi@raspi ~ $ tail -f /tmp/rolling-2020-12-31.log | grep --line-buffered F-00000001-000000000002-00
2020-12-31 11:23:19,695 DEBUG [Timer-0] d.a.s.u.GuardedTimerTask [GuardedTimerTask.java:54] F-00000001-000000000002-00: Executing timer task name=PollPowerMeter id=26904981
2020-12-31 11:23:19,696 DEBUG [Timer-0] d.a.s.h.HttpTransactionExecutor [HttpTransactionExecutor.java:107] F-00000001-000000000002-00: Sending GET request url=http://wasserversorgung//cm?cmnd=Status%208
2020-12-31 11:23:19,841 DEBUG [Timer-0] d.a.s.h.HttpTransactionExecutor [HttpTransactionExecutor.java:168] F-00000001-000000000002-00: Response code is 200
```

## Log auf Fehler durchsuchen

Bei Verdacht auf Fehlverhalten des *Smart Appliance Enabler* sollte das Log gezielt nach Fehlern durchsucht werden.
Der *Smart Appliance Enabler* loggt Fehler mit dem Log-Level `ERROR`. d.h. diese "Wort" kann man als Such-Kriterium verwenden.

```console
sae@raspi:~ $ grep ERROR /tmp/rolling-2020-12-31.log
```

Wenn dieser Befehl keine Ausgaben liefen, enthält das Log fürr diesen Tag keine Fehler. 
Zusammen mit dem Fehler wird meist die Ursache des Fehlers in das Log geschrieben. Um diese Fehlerursache mit anzuzeigen muss der Befehl erweitert werden, sodass er die 2 Zeilen vor dem Fehler und 35 Zeilen nach dem Fehler ebenfalls ausgibt:

```console
sae@raspi:~ $ grep ERROR -B 2 -A 35 /tmp/rolling-2020-12-31.log
2020-12-31 16:48:56,748 DEBUG [Timer-0] d.a.s.u.GuardedTimerTask [GuardedTimerTask.java:54] F-00000001-000000000006-00: Executing timer task name=PollPowerMeter id=25032537
2020-12-31 16:48:56,749 DEBUG [Timer-0] d.a.s.h.HttpTransactionExecutor [HttpTransactionExecutor.java:107] F-00000001-000000000006-00: Sending GET request url=http://thermomix/cm?cmnd=Status%208
2020-12-31 16:49:03,484 ERROR [Timer-0] d.a.s.h.HttpTransactionExecutor [HttpTransactionExecutor.java:120] F-00000001-000000000006-00: Error executing GET request.
java.net.SocketTimeoutException: Read timed out
        at java.base/java.net.SocketInputStream.socketRead0(Native Method)
        at java.base/java.net.SocketInputStream.socketRead(SocketInputStream.java:115)
        at java.base/java.net.SocketInputStream.read(SocketInputStream.java:168)
        at java.base/java.net.SocketInputStream.read(SocketInputStream.java:140)
        at org.apache.http.impl.io.SessionInputBufferImpl.streamRead(SessionInputBufferImpl.java:137)
        at org.apache.http.impl.io.SessionInputBufferImpl.fillBuffer(SessionInputBufferImpl.java:153)
        at org.apache.http.impl.io.SessionInputBufferImpl.readLine(SessionInputBufferImpl.java:280)
        at org.apache.http.impl.conn.DefaultHttpResponseParser.parseHead(DefaultHttpResponseParser.java:138)
        at org.apache.http.impl.conn.DefaultHttpResponseParser.parseHead(DefaultHttpResponseParser.java:56)
        at org.apache.http.impl.io.AbstractMessageParser.parse(AbstractMessageParser.java:259)
        at org.apache.http.impl.DefaultBHttpClientConnection.receiveResponseHeader(DefaultBHttpClientConnection.java:163)
        at org.apache.http.impl.conn.CPoolProxy.receiveResponseHeader(CPoolProxy.java:157)
        at org.apache.http.protocol.HttpRequestExecutor.doReceiveResponse(HttpRequestExecutor.java:273)
        at org.apache.http.protocol.HttpRequestExecutor.execute(HttpRequestExecutor.java:125)
        at org.apache.http.impl.execchain.MainClientExec.execute(MainClientExec.java:272)
        at org.apache.http.impl.execchain.ProtocolExec.execute(ProtocolExec.java:186)
        at org.apache.http.impl.execchain.RetryExec.execute(RetryExec.java:89)
        at org.apache.http.impl.execchain.RedirectExec.execute(RedirectExec.java:110)
        at org.apache.http.impl.client.InternalHttpClient.doExecute(InternalHttpClient.java:185)
        at org.apache.http.impl.client.CloseableHttpClient.execute(CloseableHttpClient.java:83)
        at org.apache.http.impl.client.CloseableHttpClient.execute(CloseableHttpClient.java:108)
        at de.avanux.smartapplianceenabler.http.HttpTransactionExecutor.get(HttpTransactionExecutor.java:116)
        at de.avanux.smartapplianceenabler.http.HttpTransactionExecutor.executeLeaveOpen(HttpTransactionExecutor.java:96)
        at de.avanux.smartapplianceenabler.http.HttpTransactionExecutor.execute(HttpTransactionExecutor.java:76)
        at de.avanux.smartapplianceenabler.http.HttpHandler.getValue(HttpHandler.java:85)
        at de.avanux.smartapplianceenabler.http.HttpHandler.getFloatValue(HttpHandler.java:45)
        at de.avanux.smartapplianceenabler.meter.HttpElectricityMeter.getValue(HttpElectricityMeter.java:281)
        at de.avanux.smartapplianceenabler.meter.HttpElectricityMeter.pollPower(HttpElectricityMeter.java:259)
        at de.avanux.smartapplianceenabler.meter.PollPowerMeter.addValue(PollPowerMeter.java:70)
        at de.avanux.smartapplianceenabler.meter.PollPowerMeter$1.runTask(PollPowerMeter.java:54)
        at de.avanux.smartapplianceenabler.util.GuardedTimerTask.run(GuardedTimerTask.java:57)
        at java.base/java.util.TimerThread.mainLoop(Timer.java:556)
        at java.base/java.util.TimerThread.run(Timer.java:506)
2020-12-31 16:49:03,487 DEBUG [Timer-0] d.a.s.n.NotificationHandler [NotificationHandler.java:96] F-00000001-000000000006-00: Checking notification preconditions: errorCountPerDay=0
```

## Version des Smart Appliance Enabler

Direkt nach dem Start loggt der *Smart Appliance Enabler* seine Version. Mit folgendem Befehl kann man diesen Eintrag anzeigen:
```console
sae@raspi:~ $ grep "Running version" /tmp/rolling-2020-12-31.log 
2020-12-31 14:36:22,435 INFO [main] d.a.s.Application [Application.java:49] Running version 1.6.7 2020-12-31 13:31
```

## Anzeige der SEMP-URL

Direkt nach dem Start loggt der *Smart Appliance Enabler* die [SEMP-URL](SEMP_DE.md), welche er an den *Sunny Home Manager* kommuniziert. Mit folgendem Befehl kann man diesen Eintrag anzeigen:
```console
sae@raspi:~ $ grep "SEMP UPnP" /tmp/rolling-2020-12-31.log
2020-12-31 14:36:22,744 INFO [main] d.a.s.s.d.SempDiscovery [SempDiscovery.java:57] SEMP UPnP will redirect to http://192.168.1.1:8080
```

## Schaltbefehl vom Sunny Home Manager
<a name="control-request">

Wenn ein Schaltbefehl vom *Sunny Home Manager* für ein Gerät empfangen wird, führt das zu einem entsprechenden Log-Eintrag, der mit folgendem Befehl angezeigt werden kann:

```console
sae@raspi:~ $ grep "control request" /tmp/rolling-2020-12-30.log
2020-12-30 14:30:09,977 DEBUG [http-nio-8080-exec-9] d.a.s.s.w.SempController [SempController.java:235] F-00000001-000000000019-00: Received control request: on=true, recommendedPowerConsumption=22000W
```
