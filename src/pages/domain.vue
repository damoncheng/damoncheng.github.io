<template>
    <div>

        <!--
            展示domain的markdown
        -->
        <Markdown :markdown_name="markdown_name">
        </Markdown>

        <!--
            展示所有module的markdown
        -->

    </div>
</template>

<script>

import Markdown from "components/Markdown";

export default {
  name: 'domain',
  components: {
      Markdown : Markdown
  },
  data () {
    return {
        domain: this.$route.params.domain
    }
  },
  methods: {
    
  },
  computed: {

      markdown_name () {
          return `${this.domain}/${this.$store.state.markdown.readme}`
      }

  },
  updated (){
    

  }
}
</script>

<style>
</style>
