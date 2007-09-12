<?xml version="1.0"?>
<maplint:tests xmlns:maplint="http://maplint.openstreetmap.org/xml/1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<maplint:test group="base" id="empty-tag-key" version="1" severity="error">

    <maplint:desc xml:lang="en">
        Elements with empty tag keys.
    </maplint:desc>

    <maplint:desc xml:lang="de">
        Elemente mit leerem Schlüsselwort im Tag.
    </maplint:desc>

    <maplint:garmin short="NOKEY" icon="Navaid, Red"/>

    <maplint:check data="any" type="application/xsl+xml">
        <xsl:if test="tag[@k='']">
            <maplint:result>Value=<xsl:value-of select="tag[@k='']/@v"/></maplint:result>
        </xsl:if>
    </maplint:check>

</maplint:test>
<maplint:test group="base" id="empty-tag-value" version="1" severity="warning">

    <maplint:desc xml:lang="en">
        Elements with empty tag value.
    </maplint:desc>

    <maplint:garmin short="NOVAL" icon="Navaid, Red"/>

    <maplint:check data="any" type="application/xsl+xml">
        <xsl:if test="tag[@v='']">
            <maplint:result>Key=<xsl:value-of select="tag[@v='']/@k"/></maplint:result>
        </xsl:if>
    </maplint:check>

</maplint:test>
<maplint:test group="base" id="nodes-on-same-spot" version="1" severity="error">

    <maplint:desc xml:lang="en">
        Two or more nodes with the exact same coordinates. This will be
        reported for every node in the set, so if there are three nodes
        with the same coordinates, there will be three reports, not one.
    </maplint:desc>

    <maplint:garmin short="DBLNOD" icon="Navaid, Violet"/>

    <maplint:setup type="application/xsl+xml">
        <xsl:key name="nodesbycoordinates" match="/osm/node" use="concat(@lon,' ', @lat)"/>
    </maplint:setup>

    <maplint:check data="node" type="application/xsl+xml">
        <xsl:variable name="nodes" select="key('nodesbycoordinates', concat(@lon, ' ', @lat))"/>
        <xsl:variable name="nid" select="@id"/>

        <xsl:if test="count($nodes) != 1">
            <maplint:result>
                <xsl:text>Nodes:</xsl:text>
                <xsl:for-each select="$nodes">
                    <xsl:if test="@id != $nid">
                        <xsl:value-of select="concat(' ', @id)"/>
                    </xsl:if>
                </xsl:for-each>
            </maplint:result>
        </xsl:if>
    </maplint:check>

</maplint:test>
<maplint:test group="base" id="untagged-way" version="1" severity="warning">

    <maplint:desc xml:lang="en">
        Way without any tags (except "created_by").
    </maplint:desc>

    <maplint:check data="way" type="application/xsl+xml">
        <xsl:if test="not(tag[@k != 'created_by'])">
            <maplint:result/>
        </xsl:if>
    </maplint:check>

</maplint:test>
<maplint:test group="main" id="bridge-or-tunnel-without-layer" version="1" severity="warning">

    <maplint:desc xml:lang="en">
        Find ways with bridge or tunnel tag without a layer tag. In most cases this is an error, but it is only tagged as warning because
        the crossing way could have a layer tag.
    </maplint:desc>

    <maplint:check data="way" type="application/xsl+xml">
        <xsl:if test="(tag[(@k='bridge' or @k='tunnel') and @v='true']) and not(tag[@k='layer'])">
            <maplint:result/>
        </xsl:if>
    </maplint:check>

</maplint:test>
<maplint:test group="main" id="deprecated-tags" version="1" severity="error">

    <maplint:desc xml:lang="en">
        Find deprecated tags: class=*
    </maplint:desc>

    <maplint:garmin short="DEPTAG" icon="Shipwreck"/>

    <maplint:check data="any" type="application/xsl+xml">
        <xsl:if test="tag/@k='class'">
            <maplint:result>class</maplint:result>
        </xsl:if>
    </maplint:check>

</maplint:test>
<maplint:test group="main" id="motorway-without-ref" version="1" severity="error">

    <maplint:desc xml:lang="en">
        This test finds all motorways (highway=motorway) without a ref tag.
    </maplint:desc>

    <maplint:garmin short="MOTNOREF" icon="Navaid, White"/>

    <maplint:check data="way" type="application/xsl+xml">
        <xsl:if test="tag[@k='highway' and @v='motorway']">
            <xsl:if test="not(tag[@k='ref'])">
                <maplint:result/>
            </xsl:if>
        </xsl:if>
    </maplint:check>

</maplint:test>
<maplint:test group="main" id="place-of-worship-without-religion" version="1" severity="warning">

    <maplint:desc xml:lang="en">
        Place of worship without associated religion key.
    </maplint:desc>

    <maplint:garmin short="PWNOREL" icon="Navaid, White"/>

    <maplint:check data="node" type="application/xsl+xml">
        <xsl:if test="(tag[@k='amenity' and @v='place_of_worship']) and not(tag[@k='religion'])">
            <maplint:result/>
        </xsl:if>
    </maplint:check>

</maplint:test>
<maplint:test group="main" id="poi-without-name" version="1" severity="warning">

    <maplint:desc xml:lang="en">
        Point of Interest such as as church, cinema, or pharmacy without a name.
    </maplint:desc>

    <maplint:garmin short="POINONAME" icon="Navaid, White"/>

    <maplint:check data="node" type="application/xsl+xml">
        <xsl:if test="(tag[@k='amenity' and (@v='place_of_worship' or @v='cinema' or @v='pharmacy' or @v='pub' or @v='restaurant' or @v='school' or @v='university' or @v='hospital' or @v='library' or @v='theatre' or @v='courthouse' or @v='bank')]) and not(tag[@k='name'])">
            <maplint:result>
                <xsl:text>amenity=</xsl:text>
                <xsl:value-of select="tag[@k='amenity']/@v"/>
            </maplint:result>
        </xsl:if>
    </maplint:check>

</maplint:test>
<maplint:test group="main" id="residential-without-name" version="1" severity="warning">

    <maplint:desc xml:lang="en">
        Find ways with tag highway=residential, but without a name tag.
    </maplint:desc>

    <maplint:garmin short="RESNONAME" icon="Navaid, White"/>

    <maplint:check data="way" type="application/xsl+xml">
        <xsl:if test="(tag[@k='highway' and @v='residential']) and not(tag[@k='name'])">
            <maplint:result/>
        </xsl:if>
    </maplint:check>

</maplint:test>
<maplint:test group="relations" id="member-missing" version="1" severity="error">

    <maplint:desc xml:lang="en">
        A member of a relation is missing.
    </maplint:desc>

    <maplint:setup type="application/xsl+xml">
        <xsl:key name="nodeId" match="/osm/node" use="@id"/>
        <xsl:key name="wayId" match="/osm/way" use="@id"/>
        <xsl:key name="relId" match="/osm/relation" use="@id"/>
    </maplint:setup>

    <maplint:check data="relation" type="application/xsl+xml">
        <xsl:if test="member[(@type='way') and not(key('wayId', @ref))]">
            <maplint:result/>
        </xsl:if>
        <xsl:if test="member[(@type='node') and not(key('nodeId', @ref))]">
            <maplint:result/>
        </xsl:if>
        <xsl:if test="member[(@type='relation') and not(key('relId', @ref))]">
            <maplint:result/>
        </xsl:if>
    </maplint:check>

</maplint:test>
<maplint:test group="segments" id="multiple-segments-on-same-nodes" version="1" severity="error">

    <maplint:desc xml:lang="en">
        Find segments which use the same nodes. Either with the same direction
        or with different directions.
    </maplint:desc>

    <maplint:setup type="application/xsl+xml">
        <xsl:key name="fromto2segment" match="/osm/segment" use="concat(@from, ' ', @to)"/>
        <xsl:key name="tofrom2segment" match="/osm/segment" use="concat(@to, ' ', @from)"/>
    </maplint:setup>

    <maplint:check data="segment" type="application/xsl+xml">
        <xsl:variable name="segment-samedir" select="key('fromto2segment', concat(@from, ' ', @to))"/>
        <xsl:variable name="segment-otherdir" select="key('tofrom2segment', concat(@to, ' ', @from))"/>
        <xsl:variable name="sid" select="@id"/>
        <xsl:if test="count($segment-samedir) &gt; 1">
            <maplint:result>
                <xsl:text>Segments with same @from/@to:</xsl:text>
                <xsl:for-each select="$segment-samedir">
                    <xsl:if test="@id != $sid">
                        <xsl:value-of select="concat(' ', @id)"/>
                    </xsl:if>
                </xsl:for-each>
            </maplint:result>
        </xsl:if>
        <xsl:if test="count($segment-otherdir) &gt; 1">
                <xsl:text>Segments with @from/@to reversed:</xsl:text>
                <xsl:for-each select="$segment-otherdir">
                    <xsl:if test="@id != $sid">
                        <xsl:value-of select="concat(' ', @id)"/>
                    </xsl:if>
                </xsl:for-each>
        </xsl:if>
    </maplint:check>

</maplint:test>
<maplint:test group="segments" id="segment-with-from-equals-to" version="1" severity="error">

    <maplint:desc xml:lang="en">
        Finds segments going from a node to the same node.
    </maplint:desc>

    <maplint:check data="segment" type="application/xsl+xml">
        <xsl:if test="@from=@to">
            <maplint:result/>
        </xsl:if>
    </maplint:check>

</maplint:test>
<maplint:test group="segments" id="segment-without-way" version="1" severity="warning">

    <maplint:desc xml:lang="en">
        Find segments which don't belong to any way. That is not
        necessarily an error, but helps with finding places where
        more works need to be done.
    </maplint:desc>

    <maplint:garmin short="SEGNOWAY" icon="Navaid, Blue"/>

    <maplint:setup type="application/xsl+xml">
        <xsl:key name="segment2way" match="/osm/way" use="seg/@id"/>
    </maplint:setup>

    <maplint:check data="segment" type="application/xsl+xml">
        <xsl:if test="not(key('segment2way', @id))">
            <maplint:result/>
        </xsl:if>
    </maplint:check>

</maplint:test>
<maplint:test group="segments" id="tagged-segment" version="1" severity="error">

    <maplint:desc xml:lang="en">
        Segment with any tag (except "created_by"). Tagging segments is
        deprecated. Please create ways and tag them instead.
    </maplint:desc>

    <maplint:check data="segment" type="application/xsl+xml">
        <xsl:if test="tag[(@k!='created_by') and (@k!='converted_by')]">
            <maplint:result>
                <xsl:for-each select="tag[(@k!='created_by') and (@k!='converted_by')]">
                    <xsl:value-of select="concat(@k,'=',@v,' ')"/>
                </xsl:for-each>
            </maplint:result>
        </xsl:if>
    </maplint:check>

</maplint:test>
<maplint:test group="segments" id="untagged-unconnected-node" version="1" severity="warning">

    <maplint:desc xml:lang="en">
        Nodes without any tags (except with key "created_by") which are not
        connected to any other nodes by segments.
    </maplint:desc>

    <maplint:setup type="application/xsl+xml">
        <xsl:key name="node-from" match="/osm/segment" use="@from"/>
        <xsl:key name="node-to" match="/osm/segment" use="@to"/>
    </maplint:setup>

    <maplint:check data="node" type="application/xsl+xml">
        <xsl:if test="not(tag[@k != 'created_by'] or key('node-from', @id) or key('node-to', @id))">
            <maplint:result/>
        </xsl:if>
    </maplint:check>

</maplint:test>
<maplint:test group="segments" id="ways-with-unordered-segments" version="1" severity="error">

    <maplint:desc xml:lang="en">
        Ways in which the segments are not ordered and sequenced properly.
    </maplint:desc>

    <maplint:setup type="application/xsl+xml">
        <xsl:key name="segment" match="/osm/segment" use="@id"/>
    </maplint:setup>

    <maplint:check data="way" type="application/xsl+xml">
        <xsl:variable name="error">
            <xsl:for-each select="seg">
                <xsl:if test="position() != last()">
                    <xsl:variable name="thissegment" select="key('segment',@id)"/>
                    <xsl:variable name="next" select="position()+1"/>
                    <xsl:variable name="nextsegment" select="key('segment',../seg[$next]/@id)"/>
                    <xsl:variable name="to" select="$thissegment/@to"/>
                    <xsl:variable name="from" select="$nextsegment/@from"/>
                    <xsl:if test="$to != $from">
                        <xsl:text>fail</xsl:text>
                    </xsl:if>
                </xsl:if>
            </xsl:for-each>
        </xsl:variable>

        <xsl:if test="$error != ''">
            <maplint:result/>
        </xsl:if>
    </maplint:check>

</maplint:test>
<maplint:test group="strict" id="unknown-tags" version="1" severity="notice">

    <maplint:desc xml:lang="en">
        Unknown tags. This is not an error as everybody can invent
        new tags. But it helps in finding typos etc.
        The list of known tags is currently incomplete.
    </maplint:desc>

    <maplint:garmin short="UNKNTAG" icon="Skull and Crossbones"/>

    <maplint:check data="node" type="application/xsl+xml">
        <xsl:for-each select="tag">
            <xsl:if test="(@k!='created_by') and                     not(starts-with(@k, 'tiger:')) and                     (@k!='converted_by') and                     (@k!='todo') and                     (@k!='landuse') and                     (@k!='note') and                     (@k!='highway') and                     (@k!='railway') and                     (@k!='waterway') and                     (@k!='amenity') and                     (@k!='dispensing') and                     (@k!='religion') and                     (@k!='military') and                     (@k!='denomination') and                     (@k!='leisure') and                     (@k!='recycling:glass') and                     (@k!='recycling:batteries') and                     (@k!='recycling:clothes') and                     (@k!='recycling:paper') and                     (@k!='recycling:green_waste') and                     (@k!='tourism') and                     (@k!='int_name') and                     (@k!='nat_name') and                     (@k!='reg_name') and                     (@k!='loc_name') and                     (@k!='old_name') and                     (@k!='int_ref') and                     (@k!='nat_ref') and                     (@k!='reg_ref') and                     (@k!='loc_ref') and                     (@k!='old_ref') and                     (@k!='ncn_ref') and                     (@k!='ele') and                     (@k!='man_made') and                     (@k!='sport') and                     (@k!='place') and                     (@k!='historic') and                     (@k!='natural') and                     (@k!='layer') and                     (@k!='religion') and                     (@k!='denomination') and                     (@k!='source') and                     (@k!='source:ref') and                     (@k!='source:name') and                     (@k!='is_in') and                     (@k!='time') and                     (@k!='access') and                     (@k!='name')">
                <maplint:result><xsl:value-of select="concat(@k, '=', @v)"/></maplint:result>
            </xsl:if>
        </xsl:for-each>
    </maplint:check>

    <maplint:check data="way" type="application/xsl+xml">
        <xsl:for-each select="tag">
            <xsl:if test="(@k!='created_by') and                     not(starts-with(@k, 'tiger:')) and                     (@k!='converted_by') and                     (@k!='highway') and                     (@k!='railway') and                     (@k!='waterway') and                     (@k!='amenity') and                     (@k!='tourism') and                     (@k!='ele') and                     (@k!='man_made') and                     (@k!='sport') and                     (@k!='place') and                     (@k!='note') and                     (@k!='historic') and                     (@k!='landuse') and                     (@k!='oneway') and                     (@k!='bridge') and                     (@k!='tunnel') and                     (@k!='leisure') and                     (@k!='junction') and                     (@k!='ref') and                     (@k!='int_name') and                     (@k!='nat_name') and                     (@k!='reg_name') and                     (@k!='loc_name') and                     (@k!='old_name') and                     (@k!='int_ref') and                     (@k!='nat_ref') and                     (@k!='reg_ref') and                     (@k!='loc_ref') and                     (@k!='old_ref') and                     (@k!='ncn_ref') and                     (@k!='natural') and                     (@k!='layer') and                     (@k!='source') and                     (@k!='source:ref') and                     (@k!='source:name') and                     (@k!='time') and                     (@k!='abutters') and                     (@k!='maxspeed') and                     (@k!='access') and                     (@k!='foot') and                     (@k!='bicycle') and                     (@k!='motorcycle') and                     (@k!='motorcar') and                     (@k!='horse') and                     (@k!='surface') and                     (@k!='osmarender:renderName') and                     (@k!='osmarender:nameDirection') and                     (@k!='name')">

                <maplint:result><xsl:value-of select="concat(@k, '=', @v)"/></maplint:result>
            </xsl:if>
        </xsl:for-each>
    </maplint:check>

</maplint:test>
</maplint:tests>
